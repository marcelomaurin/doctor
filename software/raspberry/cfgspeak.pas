unit cfgspeak;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  toolsfalar, setmain;

type

  { Tfrmcfgspeak }

  Tfrmcfgspeak = class(TForm)
    Image1: TImage;
    Image5: TImage;
    Label1: TLabel;
    Label5: TLabel;
    procedure Image1Click(Sender: TObject);
    procedure Image1MouseEnter(Sender: TObject);
    procedure Image5Click(Sender: TObject);
    procedure Image5DblClick(Sender: TObject);
  private

  public

  end;

var
  frmcfgspeak: Tfrmcfgspeak;

implementation

{$R *.lfm}

{ Tfrmcfgspeak }

procedure Tfrmcfgspeak.Image1MouseEnter(Sender: TObject);
begin
  frmToolsfalar.Falar('Configurações de Servidor de Sintetização de Vóz, srvFalar');
  Application.ProcessMessages;
end;

procedure Tfrmcfgspeak.Image5Click(Sender: TObject);
begin
  if (FSetMain.Duploclick = 0) then
  begin
    close();
  end;
end;

procedure Tfrmcfgspeak.Image5DblClick(Sender: TObject);
begin
  if (FSetMain.Duploclick <> 0) then
  begin
    close();
  end;
end;

procedure Tfrmcfgspeak.Image1Click(Sender: TObject);
begin
  frmtoolsfalar.show();
  //frmToolsfalar.setfocus();
  Application.ProcessMessages();
end;

end.

