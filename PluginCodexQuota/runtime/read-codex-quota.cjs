const { spawn } = require("child_process");
const { existsSync, readdirSync, statSync } = require("fs");
const { join } = require("path");

function findCodex() {
  if (process.env.CODEX_QUOTA_CODEX_EXE && existsSync(process.env.CODEX_QUOTA_CODEX_EXE)) return process.env.CODEX_QUOTA_CODEX_EXE;
  const binDir = join(process.env.LOCALAPPDATA || "", "OpenAI", "Codex", "bin");
  const candidates = [];
  for (const entry of readdirSync(binDir, { withFileTypes: true })) {
    const candidate = entry.isDirectory() ? join(binDir, entry.name, "codex.exe") : entry.name.toLowerCase() === "codex.exe" ? join(binDir, entry.name) : null;
    if (candidate && existsSync(candidate)) candidates.push(candidate);
  }
  if (!candidates.length) throw new Error("Codex CLI was not found.");
  return candidates.sort((a, b) => statSync(b).mtimeMs - statSync(a).mtimeMs)[0];
}
function main() {
  const executable = findCodex();
  const child = spawn(executable, ["app-server", "--stdio"], { stdio: ["pipe", "pipe", "ignore"], windowsHide: true });
  let buffer = "", initialized = false, finished = false;
  const timeout = setTimeout(() => finish({ status: "error", message: "Timed out reading Codex quota." }), 12000);
  const finish = (result) => {
    if (finished) return;
    finished = true;
    clearTimeout(timeout);
    process.stdout.write(JSON.stringify(result) + "\n");
    child.kill();
  };
  const send = (message) => child.stdin.write(JSON.stringify(message) + "\n");
  child.stdout.on("data", (chunk) => {
    buffer += chunk.toString("utf8");
    for (;;) {
      const newline = buffer.indexOf("\n"); if (newline < 0) break;
      const line = buffer.slice(0, newline).trim(); buffer = buffer.slice(newline + 1); if (!line) continue;
      let message; try { message = JSON.parse(line); } catch (_) { continue; }
      if (message.id === 1) { if (message.error) return finish({ status: "error", message: "Codex app-server initialization failed." }); initialized = true; send({ method: "initialized", params: {} }); send({ id: 2, method: "account/read", params: { refreshToken: false } }); }
      else if (message.id === 2) { if (message.error || !message.result?.account) return finish({ status: "needs-login" }); send({ id: 3, method: "account/rateLimits/read", params: {} }); }
      else if (message.id === 3) { const limits = message.result?.rateLimits; finish(limits ? { status: "ok", primary: limits.primary, secondary: limits.secondary } : { status: "error", message: "Codex did not return quota data." }); }
    }
  });
  child.on("error", (error) => finish({ status: "error", message: error.message }));
  child.on("exit", () => { if (!finished) finish({ status: "error", message: initialized ? "Codex app-server stopped unexpectedly." : "Codex app-server did not start." }); });
  send({ id: 1, method: "initialize", params: { clientInfo: { name: "codex-quota-taskbar", version: "2.0" } } });
}
try { main(); } catch (error) { process.stdout.write(JSON.stringify({ status: "error", message: error.message }) + "\n"); }
