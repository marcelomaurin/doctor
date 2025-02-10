unit OpAmostragem;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  banco, Etiquetar, setmain;

type

  { TfrmOpAmostragem }

  TfrmOpAmostragem = class(TForm)
    Image1: TImage;
    Label1: TLabel;
    procedure Image1Click(Sender: TObject);
    procedure Image1DblClick(Sender: TObject);
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

procedure TfrmOpAmostragem.OperacaoEtiquetar;
begin
  frmEtiquetar :=  TfrmEtiquetar.create(self);

  Application.ProcessMessages;
  frmEtiquetar.showmodal();
  frmEtiquetar.free();

end;

end.

