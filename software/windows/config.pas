unit config;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ComCtrls,
  ExtCtrls, IPEdit, setmain;

type

  { Tfrmconfig }

  Tfrmconfig = class(TForm)
    btSalvar: TButton;
    cbTipoImp: TComboBox;
    edUsuario: TEdit;
    edHostname: TEdit;
    edbanco: TEdit;
    edServidor: TIPEdit;
    edTCPSERIAL: TEdit;
    edPassword: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    Label7: TLabel;
    PageControl1: TPageControl;
    Panel1: TPanel;
    TabSheet1: TTabSheet;
    TabSheet2: TTabSheet;
    tsServidor: TTabSheet;
    procedure btSalvarClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private

  public

  end;

var
  frmconfig: Tfrmconfig;

implementation

{$R *.lfm}

{ Tfrmconfig }

procedure Tfrmconfig.btSalvarClick(Sender: TObject);
begin
  FSetMain.SerialPort:= edTCPSERIAL.text;
  FSetMain.Servidor:= edServidor.text;
  FSetMain.TipoImp := cbTipoImp.ItemIndex;
  FSetMain.HostnameMy:= edHostname.text;
  FSetMain.BancoMy:= edbanco.text;
  FSetMain.UsernameMy := edUsuario.text;
  FSetMain.PasswordMy := edPassword.text;
  FSetMain.SalvaContexto(false);
  Close;
end;

procedure Tfrmconfig.FormCreate(Sender: TObject);
begin
  edTCPSERIAL.text :=   FSetMain.SerialPort;
  edServidor.text := FSetMain.Servidor;
  cbTipoImp.ItemIndex:= FSetMain.TipoImp;
  edHostname.text := FSetMain.HostnameMy;
  edbanco.text :=  FSetMain.BancoMy;
  edUsuario.text := FSetMain.UsernameMy;
  edPassword.text := FSetMain.PasswordMy;
end;

end.

