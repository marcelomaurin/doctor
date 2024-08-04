unit OpAmostragem;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  toolsfalar, banco, Etiquetar, setmain;

type

  { TfrmOpAmostragem }

  TfrmOpAmostragem = class(TForm)
    Image1: TImage;
    Image5: TImage;
    Label1: TLabel;
    Label5: TLabel;
    procedure Image1Click(Sender: TObject);
    procedure Image1DblClick(Sender: TObject);
    procedure Image5Click(Sender: TObject);
    procedure Image5DblClick(Sender: TObject);
  private
    procedure OperacaoEtiquetar();
  public

  end;

var
  frmOpAmostragem: TfrmOpAmostragem;

implementation

{$R *.lfm}

{ TfrmOpAmostragem }

procedure TfrmOpAmostragem.Image1Click(Sender: TObject);
begin
  if(FSetMain.Duploclick=0) then
  begin
   OperacaoEtiquetar();

  end;
end;

procedure TfrmOpAmostragem.Image1DblClick(Sender: TObject);
begin
  if(FSetMain.Duploclick<>0) then
  begin
   OperacaoEtiquetar();

  end;
end;

procedure TfrmOpAmostragem.Image5Click(Sender: TObject);
begin
  if(FSetMain.Duploclick=0) then
  begin
   close();
  end;

end;

procedure TfrmOpAmostragem.Image5DblClick(Sender: TObject);
begin
  if(FSetMain.Duploclick<>0) then
  begin
   close();
  end;
end;

procedure TfrmOpAmostragem.OperacaoEtiquetar;
begin
  frmEtiquetar :=  TfrmEtiquetar.create(self);
  frmToolsfalar.Falar('Operações de Etiquetagem');
  Application.ProcessMessages;
  frmEtiquetar.showmodal();
  frmEtiquetar.free();

end;

end.

