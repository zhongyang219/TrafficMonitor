#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <string>
#include "PluginInterface.h"

namespace {

HMODULE g_module = nullptr;

std::wstring ModuleDirectory() {
    wchar_t path[MAX_PATH]{};
    GetModuleFileNameW(g_module, path, MAX_PATH);
    std::wstring value(path);
    return value.substr(0, value.find_last_of(L"\\/"));
}

std::string ReadFile(const std::wstring& path) {
    std::ifstream file(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), {}};
}

std::string ReadJsonString(const std::string& text, const char* key, const char* fallback) {
    const std::string needle = std::string("\"") + key + "\"";
    size_t pos = text.find(needle);
    if (pos == std::string::npos || (pos = text.find(':', pos)) == std::string::npos) return fallback;
    pos = text.find('"', pos);
    if (pos == std::string::npos) return fallback;
    const size_t end = text.find('"', pos + 1);
    return end == std::string::npos ? fallback : text.substr(pos + 1, end - pos - 1);
}

int ReadJsonNumber(const std::string& text, const char* key) {
    const std::string needle = std::string("\"") + key + "\"";
    size_t pos = text.find(needle);
    if (pos == std::string::npos || (pos = text.find(':', pos)) == std::string::npos) return 0;
    return std::clamp(atoi(text.c_str() + pos + 1), 0, 100);
}

bool ReadJsonBool(const std::string& text, const char* key, bool fallback) {
    const std::string needle = std::string("\"") + key + "\"";
    size_t pos = text.find(needle);
    if (pos == std::string::npos || (pos = text.find(':', pos)) == std::string::npos) return fallback;
    pos = text.find_first_not_of(" \t\r\n", pos + 1);
    return pos != std::string::npos && text.compare(pos, 4, "true") == 0;
}

std::wstring Widen(const std::string& value) {
    return std::wstring(value.begin(), value.end());
}

void FillRoundRect(HDC dc, int left, int top, int right, int bottom, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    HPEN pen = CreatePen(PS_NULL, 0, color);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    RoundRect(dc, left, top, right, bottom, (std::max)(4, bottom - top), (std::max)(4, bottom - top));
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}

class CodexQuotaPlugin;

class CodexQuotaItem final : public IPluginItem {
public:
    explicit CodexQuotaItem(CodexQuotaPlugin& plugin) : plugin_(plugin) {}
    const wchar_t* GetItemName() const override;
    const wchar_t* GetItemId() const override { return L"CodexQuotaNetwork"; }
    const wchar_t* GetItemLableText() const override { return L""; }
    const wchar_t* GetItemValueText() const override { return L""; }
    const wchar_t* GetItemValueSampleText() const override { return L""; }
    bool IsCustomDraw() const override { return true; }
    int GetItemWidth() const override { return 266; }
    void DrawItem(void* hDC, int x, int y, int w, int h, bool darkMode) override;
    int OnMouseEvent(MouseEventType type, int x, int y, void* hWnd, int flag) override;
private:
    CodexQuotaPlugin& plugin_;
};

class CodexQuotaPlugin final : public ITMPlugin {
public:
    struct State {
        bool hasFiveHour = true;
        int fiveHourRemaining = 0;
        int weeklyRemaining = 0;
        std::wstring fiveHourReset = L"--:--";
        std::wstring weeklyReset = L"--/--";
        std::wstring status = L"loading";
    };

    CodexQuotaPlugin() : item_(*this) {}
    IPluginItem* GetItem(int index) override { return index == 0 ? &item_ : nullptr; }
    void OnInitialize(ITrafficMonitor* app) override { app_ = app; root_ = ModuleDirectory(); }
    void DataRequired() override {
        LoadState();
        const ULONGLONG now = GetTickCount64();
        if (now - lastRefresh_ >= static_cast<ULONGLONG>(refreshSeconds_) * 1000) RequestRefresh(false);
    }
    const wchar_t* GetInfo(PluginInfoIndex index) override {
        switch (index) {
        case TMI_NAME: return L"Codex Quota + Network";
        case TMI_DESCRIPTION: return L"Codex quota and TrafficMonitor network speed in one taskbar item.";
        case TMI_AUTHOR: return L"frey77";
        case TMI_COPYRIGHT: return L"TrafficMonitor contributors";
        case TMI_VERSION: return L"0.1.0";
        case TMI_URL: return L"https://github.com/zhongyang219/TrafficMonitor";
        default: return L"";
        }
    }
    const State& state() const { return state_; }
    ITrafficMonitor* app() const { return app_; }
    bool IsChinese() const { return app_ && PRIMARYLANGID(app_->GetLanguageId()) == LANG_CHINESE; }
    void RequestRefresh(bool notify) {
        lastRefresh_ = GetTickCount64();
        const std::wstring script = root_ + L"\\RefreshCodexQuota.ps1";
        const std::wstring arguments = L"-NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -File \"" + script + L"\"";
        const HINSTANCE result = ShellExecuteW(nullptr, L"open", L"powershell.exe", arguments.c_str(), root_.c_str(), SW_HIDE);
        if (notify && app_) app_->ShowNotifyMessage(reinterpret_cast<INT_PTR>(result) > 32 ? (IsChinese() ? L"正在刷新 Codex 额度。" : L"Refreshing Codex quota.") : (IsChinese() ? L"无法启动额度刷新器。" : L"Could not start the quota refresher."));
    }
    void StartLogin() {
        const std::wstring script = root_ + L"\\AuthorizeCodex.ps1";
        const std::wstring arguments = L"-NoProfile -ExecutionPolicy Bypass -File \"" + script + L"\"";
        ShellExecuteW(nullptr, L"open", L"powershell.exe", arguments.c_str(), root_.c_str(), SW_SHOWNORMAL);
    }
    void ShowMenu(HWND parent) {
        POINT point{}; GetCursorPos(&point);
        HMENU menu = CreatePopupMenu();
        AppendMenuW(menu, MF_STRING, 1, IsChinese() ? L"立即刷新" : L"Refresh now");
        AppendMenuW(menu, MF_STRING, 2, IsChinese() ? L"登录 Codex" : L"Sign in to Codex");
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(menu, MF_STRING | (refreshSeconds_ == 30 ? MF_CHECKED : 0), 30, IsChinese() ? L"每 30 秒刷新" : L"Refresh every 30 seconds");
        AppendMenuW(menu, MF_STRING | (refreshSeconds_ == 60 ? MF_CHECKED : 0), 60, IsChinese() ? L"每 1 分钟刷新" : L"Refresh every minute");
        AppendMenuW(menu, MF_STRING | (refreshSeconds_ == 300 ? MF_CHECKED : 0), 300, IsChinese() ? L"每 5 分钟刷新" : L"Refresh every 5 minutes");
        SetForegroundWindow(parent);
        const UINT command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, point.x, point.y, 0, parent, nullptr);
        DestroyMenu(menu);
        if (command == 1) RequestRefresh(true);
        else if (command == 2) StartLogin();
        else if (command == 30 || command == 60 || command == 300) refreshSeconds_ = command;
    }
private:
    void LoadState() {
        const std::string text = ReadFile(root_ + L"\\quota-state.json");
        if (text.empty()) return;
        state_.hasFiveHour = ReadJsonBool(text, "hasFiveHour", true);
        state_.fiveHourRemaining = ReadJsonNumber(text, "fiveHourRemaining");
        state_.weeklyRemaining = ReadJsonNumber(text, "weeklyRemaining");
        state_.fiveHourReset = Widen(ReadJsonString(text, "fiveHourReset", "--:--"));
        state_.weeklyReset = Widen(ReadJsonString(text, "weeklyReset", "--/--"));
        state_.status = Widen(ReadJsonString(text, "status", "loading"));
    }

    ITrafficMonitor* app_ = nullptr;
    CodexQuotaItem item_;
    State state_;
    std::wstring root_;
    ULONGLONG lastRefresh_ = 0;
    UINT refreshSeconds_ = 30;
    friend class CodexQuotaItem;
};

const wchar_t* CodexQuotaItem::GetItemName() const {
    return plugin_.IsChinese() ? L"Codex 额度 + 网速" : L"Codex Quota + Network";
}

void CodexQuotaItem::DrawItem(void* handle, int x, int y, int w, int h, bool darkMode) {
    HDC dc = static_cast<HDC>(handle);
    const auto& state = plugin_.state();
    const COLORREF text = darkMode ? RGB(246, 248, 250) : RGB(28, 34, 40);
    const COLORREF track = darkMode ? RGB(74, 91, 103) : RGB(186, 197, 202);
    const COLORREF weekly = RGB(106, 145, 171);
    const COLORREF shortWindow = RGB(103, 158, 147);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, text);
    HFONT font = CreateFontW(-(std::max)(11, h / 3), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Microsoft YaHei UI");
    HGDIOBJ oldFont = SelectObject(dc, font);
    const int quotaWidth = (std::min)(156, (std::max)(122, w - 104));
    const int labelWidth = 24;
    const int barX = x + labelWidth + 4;
    const int barWidth = quotaWidth - labelWidth - 36;
    const int rowHeight = (std::max)(12, h / (state.hasFiveHour ? 2 : 1) - 4);
    auto row = [&](int top, const wchar_t* label, int remaining, const std::wstring& reset, COLORREF fill) {
        TextOutW(dc, x, top, label, static_cast<int>(wcslen(label)));
        FillRoundRect(dc, barX, top + 2, barX + barWidth, top + rowHeight, track);
        const int filled = (std::max)(4, barWidth * remaining / 100);
        FillRoundRect(dc, barX, top + 2, barX + filled, top + rowHeight, fill);
        std::wstring value = std::to_wstring(remaining) + L"%";
        SetTextColor(dc, remaining >= 15 ? RGB(250, 251, 252) : text);
        RECT valueRect{barX, top + 1, barX + barWidth, top + rowHeight};
        DrawTextW(dc, value.c_str(), -1, &valueRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SetTextColor(dc, text);
        TextOutW(dc, barX + barWidth + 4, top, reset.c_str(), static_cast<int>(reset.size()));
    };
    if (state.hasFiveHour) {
        row(y + 1, L"5H", state.fiveHourRemaining, state.fiveHourReset, shortWindow);
        row(y + h / 2, L"1W", state.weeklyRemaining, state.weeklyReset, weekly);
    } else {
        row(y + (h - rowHeight) / 2, L"1W", state.weeklyRemaining, state.weeklyReset, weekly);
    }
    const int speedX = x + quotaWidth + 8;
    if (plugin_.app()) {
        const wchar_t* up = plugin_.app()->GetMonitorValueString(ITrafficMonitor::MI_UP);
        const wchar_t* down = plugin_.app()->GetMonitorValueString(ITrafficMonitor::MI_DOWN);
        std::wstring upText = L"↑ " + std::wstring(up ? up : L"--");
        std::wstring downText = L"↓ " + std::wstring(down ? down : L"--");
        TextOutW(dc, speedX, y + 1, upText.c_str(), static_cast<int>(upText.size()));
        TextOutW(dc, speedX, y + h / 2, downText.c_str(), static_cast<int>(downText.size()));
    }
    SelectObject(dc, oldFont);
    DeleteObject(font);
}

int CodexQuotaItem::OnMouseEvent(MouseEventType type, int, int, void* hWnd, int) {
    if (type == MT_LCLICKED) { plugin_.RequestRefresh(true); return 1; }
    if (type == MT_RCLICKED) { plugin_.ShowMenu(static_cast<HWND>(hWnd)); return 1; }
    return 0;
}

CodexQuotaPlugin g_plugin;

} // namespace

extern "C" __declspec(dllexport) ITMPlugin* TMPluginGetInstance() { return &g_plugin; }

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) g_module = module;
    return TRUE;
}
