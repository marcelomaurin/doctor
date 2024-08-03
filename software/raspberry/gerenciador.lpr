program gerenciador;

{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}
  cthreads,
  {$ENDIF}
  {$IFDEF HASAMIGA}
  athreads,
  {$ENDIF}
  Interfaces, // this includes the LCL widgetset
  Forms, main, setmain, toolsfalar, funcoes, lnetbase, lnetvisual,
  LazSerialPort, zcomponent, config, banco, cfgspeak, cfgBanco, bloqueio,
  OpAmostragem, imp, imp_ELGINI9, imp_generico, imp_qr203, cfgImp, Etiquetar,
  PesqPacientes, PesqTipoExames, chatgpt, ToolsOuvir, toolsver
  { you can add units after this };

{$R *.res}

begin
  RequireDerivedFormResource:=True;
  Application.Scaled:=True;
  Application.Initialize;
  Application.CreateForm(Tfrmmain, frmmain);
  Application.Run;
end.

