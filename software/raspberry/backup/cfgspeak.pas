unit cfgspeak;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  toolsfalar;

type

  { Tfrmcfgspeak }

  Tfrmcfgspeak = class(TForm)
    Image1: TImage;
    Label1: TLabel;
    procedure Image1Click(Sender: TObject);
    procedure Image1MouseEnter(Sender: TObject);
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

procedure Tfrmcfgspeak.Image1Click(Sender: TObject);
begin
  frmtoolsfalar.show();
  //frmToolsfalar.setfocus();
  Application.ProcessMessages();
end;

end.

