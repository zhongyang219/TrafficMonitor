#pragma once
#include "CommonData.h"
class CHistoryTrafficFile
{
public:
	CHistoryTrafficFile(const wstring& file_path);
	~CHistoryTrafficFile();

	void Save() const;
	void Load();
	void LoadSize();			//仅读取文件的大小

	const wstring& GetFilePath() const { return m_file_path; }
	const void SetFilePath(const wstring& file_path) { m_file_path = file_path; }
	deque<HistoryTraffic>& GetTraffics() { return m_history_traffics; }
	__int64 GetTodayUpTraffic() const { return m_today_up_traffic; }
	__int64 GetTodayDownTraffic() const { return m_today_down_traffic; }
	size_t Size() { return m_size; }

private:
	wstring m_file_path;
	deque<HistoryTraffic> m_history_traffics;	//储存历史流量
	__int64 m_today_up_traffic{};	//今天已使用的上传流量
	__int64 m_today_down_traffic{};	//今天已使用的下载流量
	size_t m_size{};				//流量数据的数量
};

