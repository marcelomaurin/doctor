unit PesqPacientes;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, DB, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  DBGrids, banco;

type

  { TfrmPesqPacientes }

  TfrmPesqPacientes = class(TForm)
    btSair: TButton;
    Button1: TButton;
    dsCliente: TDataSource;
    DBGrid1: TDBGrid;
    edPesqNome: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    Panel1: TPanel;
    Panel2: TPanel;
    Panel3: TPanel;
    procedure btSairClick(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure DBGrid1DblClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private

  public
   idCliente : integer;
   nome : string;
  end;

var
  frmPesqPacientes: TfrmPesqPacientes;

implementation

{$R *.lfm}

{ TfrmPesqPacientes }

procedure TfrmPesqPacientes.btSairClick(Sender: TObject);
begin
  Close;
end;

procedure TfrmPesqPacientes.Button1Click(Sender: TObject);
begin
  dmbanco.BuscaCliente(edPesqNome.text);

end;

procedure TfrmPesqPacientes.DBGrid1DblClick(Sender: TObject);
begin
  idCliente := dmbanco.zqryCliente.FieldByName('id').asinteger;
  Nome := dmBanco.zqryCliente.FieldByName('nome').asstring;
  close;
end;

procedure TfrmPesqPacientes.FormCreate(Sender: TObject);
begin
  idCliente:= 0;
  Nome := '';
end;

end.

