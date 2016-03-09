program PIC_downloader;

uses
  Forms,
  Main in 'Main.pas' {MainForm};

{$R *.RES}

begin
  Application.Initialize;
  Application.Title := 'PIC downloader';
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
