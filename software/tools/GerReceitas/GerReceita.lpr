program GerReceita;

{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}{$IFDEF UseCThreads}
  cthreads,
  {$ENDIF}{$ENDIF}
  Interfaces, // this includes the LCL widgetset
  Forms, lnetvisual, sdposeriallaz, pascalscript, tachartlazaruspkg, main,
  registro
  { you can add units after this };

{$R *.res}

begin
  RequireDerivedFormResource := True;
  Application.Title:='GerReceitas';
  Application.Initialize;
  Application.CreateForm(TfrmMain, frmMain);
  Application.Run;
end.

