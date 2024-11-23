unit PesqTipoExames;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, DB, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  DBGrids,  banco;

type

  { TfrmPesqTipoExames }
  TfrmPesqTipoExames = class(TForm)
    btSair: TButton;
    Button1: TButton;
    DBGrid1: TDBGrid;
    dsExame: TDataSource;
    edPesqNome: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    Panel1: TPanel;
    Panel2: TPanel;
    Panel3: TPanel;
    procedure btSairClick(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure DBGrid1DblClick(Sender: TObject);
  private

  public
     idExame : integer;
     Nome : string;
  end;

var
  frmPesqTipoExames: TfrmPesqTipoExames;

implementation

{$R *.lfm}

{ TfrmPesqTipoExames }

procedure TfrmPesqTipoExames.btSairClick(Sender: TObject);
begin
   Close;
end;

procedure TfrmPesqTipoExames.Button1Click(Sender: TObject);
begin
  dmbanco.BuscaTipoExame(edPesqNome.text);

end;

procedure TfrmPesqTipoExames.DBGrid1DblClick(Sender: TObject);
begin
  idExame := dmbanco.zqryExames.FieldByName('id').asinteger;
  Nome := dmBanco.zqryExames.FieldByName('nome').asstring;
  close;
end;

end.

