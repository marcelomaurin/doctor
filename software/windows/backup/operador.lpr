program operador;

{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}
  cthreads,
  {$ENDIF}
  {$IFDEF HASAMIGA}
  athreads,
  {$ENDIF}
  Interfaces, // this includes the LCL widgetset
  Forms, lnetbase, sdposeriallaz, LazSerialPort, indylaz, zcomponent, main,
  PesqTipoExames, PesqPacientes, OpAmostragem, imp_qr203, imp_generico,
  imp_ELGINI9, imp, brobotico, bloqueio, banco, setmain, funcoes, Etiquetar,
  receitas, config
  { you can add units after this };

{$R *.res}

begin
  RequireDerivedFormResource:=True;
  Application.Scaled:=True;
  Application.Initialize;
  Application.CreateForm(Tfrmmain, frmmain);
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.

