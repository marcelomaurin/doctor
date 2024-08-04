unit cfgBanco;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  toolsfalar;

type

  { TfrmcfgBanco }

  TfrmcfgBanco = class(TForm)
    Image5: TImage;
    Label5: TLabel;
    procedure FormClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure Image5Click(Sender: TObject);
    procedure Image5DblClick(Sender: TObject);
  private

  public

  end;

var
  frmcfgBanco: TfrmcfgBanco;

implementation

{$R *.lfm}

{ TfrmcfgBanco }

procedure TfrmcfgBanco.FormShow(Sender: TObject);
begin
  frmToolsfalar.Falar('Nesta tela serão configurados os acessos ao banco de dados');
  Application.ProcessMessages;
end;

procedure TfrmcfgBanco.Image5Click(Sender: TObject);
begin
  if (FSetMain.Duploclick = 0) then
  begin
    close();
  end;
end;

procedure TfrmcfgBanco.Image5DblClick(Sender: TObject);
begin
  if (FSetMain.Duploclick <> 0) then
  begin
    close();
  end;
end;

procedure TfrmcfgBanco.FormClick(Sender: TObject);
begin

end;

end.

