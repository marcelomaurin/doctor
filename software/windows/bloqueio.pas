unit bloqueio;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ExtCtrls,
  banco;

type

  { TfrmBloqueio }

  TfrmBloqueio = class(TForm)
    edChave: TEdit;
    Image1: TImage;
    Label1: TLabel;
    lbInvalido: TLabel;
    procedure edChaveKeyPress(Sender: TObject; var Key: char);
  private

  public

  end;

var
  frmBloqueio: TfrmBloqueio;

implementation

{$R *.lfm}

{ TfrmBloqueio }

procedure TfrmBloqueio.edChaveKeyPress(Sender: TObject; var Key: char);
begin
  if (key = #13) then
  begin
    if dmbanco.BuscaChavePessoa(edChave.text) then
    begin
      lbInvalido.Visible:= false;
      close;

    end
    else
    begin
      edChave.text := '';
      lbInvalido.Visible:= true;
    end;
  end;
end;

end.

