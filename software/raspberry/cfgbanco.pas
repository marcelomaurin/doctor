unit cfgBanco;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, toolsfalar;

type

  { TfrmcfgBanco }

  TfrmcfgBanco = class(TForm)
    procedure FormShow(Sender: TObject);
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

end.

