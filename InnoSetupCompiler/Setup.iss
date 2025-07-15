[Code]
// Function to check if the app is already installed by reading the registry
function InitializeSetup(): Boolean;
var
  InstalledVersion: string;
begin
  // Check if the app is already installed by querying the registry
  if RegQueryStringValue(HKCU, 'Software\MyCompany\TrafficMonitor', 'Version', InstalledVersion) then
  begin
    MsgBox('A previous version (' + InstalledVersion + ') of Traffic Monitor is already installed. Installation will proceed with the new version.', mbInformation, MB_OK);
  end
  else
  begin
    MsgBox('No previous version found. Proceeding with installation of Traffic Monitor.', mbInformation, MB_OK);
  end;

  Result := True;  // Allow setup to continue
end;
#define MyAppName "Traffic Monitor"
#define MyAppVersion "1.85.1"
#define MyAppPublisher "zhongyang219"
#define MyAppURL "https://github.com/zhongyang219/TrafficMonitor"
// #define groupname ""
#define OutputDir "./output"
#define IconFile "./TrafficMonitor.ico"

[Setup]
AppId={{B07BBDF1-7889-4816-A970-E43D611A24D1}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
ArchitecturesInstallIn64BitMode="x64os"
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
OutputDir={#OutputDir}
OutputBaseFilename={#MyAppName}_{#MyAppVersion}
SetupIconFile={#IconFile}
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "arabic"; MessagesFile: "compiler:Languages\Arabic.isl"
Name: "armenian"; MessagesFile: "compiler:Languages\Armenian.isl"
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "bulgarian"; MessagesFile: "compiler:Languages\Bulgarian.isl"
Name: "catalan"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "corsican"; MessagesFile: "compiler:Languages\Corsican.isl"
Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "danish"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "finnish"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "hebrew"; MessagesFile: "compiler:Languages\Hebrew.isl"
Name: "hungarian"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"
Name: "korean"; MessagesFile: "compiler:Languages\Korean.isl"
Name: "norwegian"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "polish"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "slovak"; MessagesFile: "compiler:Languages\Slovak.isl"
Name: "slovenian"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "swedish"; MessagesFile: "compiler:Languages\Swedish.isl"
Name: "tamil"; MessagesFile: "compiler:Languages\Tamil.isl"
Name: "turkish"; MessagesFile: "compiler:Languages\Turkish.isl"
Name: "ukrainian"; MessagesFile: "compiler:Languages\Ukrainian.isl"

[Registry]
Root: HKCU; Subkey: "Software\MyCompany\TrafficMonitor"; ValueName: "InstallPath"; ValueType: string; ValueData: "{app}"
Root: HKCU; Subkey: "Software\MyCompany\TrafficMonitor"; ValueName: "Version"; ValueType: string; ValueData: "{#MyAppVersion}"

[Files]
Source: "Source\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\TrafficMonitor.exe"; WorkingDir: "{app}"; IconFilename: "{app}\TrafficMonitor.ico"; Tasks: desktopicon
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\TrafficMonitor.exe"; WorkingDir: "{app}"; IconFilename: "{app}d\TrafficMonitor.ico"; Tasks: startmenuicon

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional tasks";
Name: "startmenuicon"; Description: "Create a Start Menu shortcut"; GroupDescription: "Additional tasks";

[Messages]
WelcomeMessage=Welcome to the {#MyAppName} {#MyAppVersion} Setup Wizard!
ExitMessage=Thank you for installing {#MyAppName}. We hope you enjoy using it!

[UninstallDelete]
Type: files; Name: "{app}\TrafficMonitor.exe"
Type: dirifempty; Name: "{app}"

; [Setup]
; CustomBackgroundFile=./background.bmp


