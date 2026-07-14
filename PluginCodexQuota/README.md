# Codex Quota + Network plugin

This optional TrafficMonitor plugin renders the local Codex quota on the left and TrafficMonitor's host-provided upload/download speed on the right in one custom taskbar item. It does not change TrafficMonitor's core monitoring code.

## Build and install

1. Run `build.cmd` with Visual Studio 2022 Build Tools.
2. Copy every file from `dist` directly into the TrafficMonitor `plugins` directory.
3. Restart TrafficMonitor and enable **Codex Quota + Network** in the taskbar display items. Disable the built-in upload/download items to avoid duplicate speed text.

The plugin refreshes local Codex quota through the local read-only app-server interface. Left-click refreshes now; right-click offers refresh, sign-in, and interval settings. If the temporary 5H quota window is unavailable, only the centered 1W row is drawn.

Prebuilt releases and standalone documentation: https://github.com/frey77/codex-quota-trafficmonitor-plugin

## License

This directory is contributed under the repository's [Anti-996 License](../LICENSE).
