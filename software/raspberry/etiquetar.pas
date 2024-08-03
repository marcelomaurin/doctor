unit Etiquetar;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, DB, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  DBCtrls, banco, setmain, imp, PesqPacientes, PesqTipoExames;

type

  { TfrmEtiquetar }

  TfrmEtiquetar = class(TForm)
    btPesquisarPaciente: TButton;
    btPesquisarPaciente1: TButton;
    dsEtiqueta: TDataSource;
    edIdCliente: TEdit;
    edidExame: TEdit;
    Image1: TImage;
    Image2: TImage;
    Label1: TLabel;
    Label2: TLabel;
    lbNome: TLabel;
    lbExameNome: TLabel;
    procedure btPesquisarPaciente1Click(Sender: TObject);
    procedure btPesquisarPacienteClick(Sender: TObject);
    procedure mage2Click(Sender: TObject);
    procedure Image2DblClick(Sender: TObject);
  private

  public
    procedure ImprimirEtiqueta();
  end;

var
  frmEtiquetar: TfrmEtiquetar;

implementation


{$R *.lfm}

{ TfrmEtiquetar }

uses main;

procedure TfrmEtiquetar.btPesquisarPacienteClick(Sender: TObject);
begin
   frmPesqPacientes := TfrmPesqPacientes.create(self);
   frmPesqPacientes.ShowModal;
   if (frmPesqPacientes.idCliente<>0) then
   begin
       edIdCliente.text := inttostr(frmPesqPacientes.idCliente);
       lbNome.Caption:= frmPesqPacientes.Nome;
   end;

end;

procedure TfrmEtiquetar.mage2Click(Sender: TObject);
begin

end;

procedure TfrmEtiquetar.Image2DblClick(Sender: TObject);
begin
  ImprimirEtiqueta();
end;

procedure TfrmEtiquetar.ImprimirEtiqueta;
var
  ind : integer;
begin
   if (TImp <> nil) then
   begin
       //Registra
       ind := dmbanco.RegistraAmostra(edIdCliente.text,edidExame.text);
       //Imprime
       FImp.Tipoimp :=   TI_SERIAL;
       FIMP.modeloimp := MI_IMP59COL;
       FIMP.COLUNA := 30;
       fimp.close;
       fimp.Device:= fsetmain.ComPrinter;
       fimp.Open;
       fimp.TextoSerial(trim(lbNome.Caption),FLEFT,TT_NORMAL);
       fimp.TextoSerial(trim(lbExameNome.Caption)+ ' '+ DateTimeToStr(now));
       fimp.PrintBARCODE(inttostr(ind));
       fimp.beep();
       //fimp.EjetarCUPOM();
       //fimp.Guilhotina();
       fimp.close;
   end;
end;

procedure TfrmEtiquetar.btPesquisarPaciente1Click(Sender: TObject);
begin
   frmPesqTipoExames := TfrmPesqTipoExames.create(self);
   frmPesqTipoExames.ShowModal;
   if (frmPesqTipoExames.idExame<>0) then
   begin
       edidExame.text := inttostr(frmPesqTipoExames.idExame);
       lbExameNome.Caption:= frmPesqTipoExames.Nome;
   end;
end;

end.

