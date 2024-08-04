unit cfgImp;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ExtCtrls,
  banco, imp, setmain, funcoes;

type

  { TfrmcfgImp }

  TfrmcfgImp = class(TForm)
    Button1: TButton;
    ckDuploClick: TCheckBox;
    edComPrinter: TEdit;
    Image5: TImage;
    Label1: TLabel;
    Label5: TLabel;
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure Image5Click(Sender: TObject);
    procedure Image5DblClick(Sender: TObject);
  private

  public

  end;

var
  frmcfgImp: TfrmcfgImp;

implementation

{$R *.lfm}

{ TfrmcfgImp }

procedure TfrmcfgImp.FormCreate(Sender: TObject);
begin
   edComPrinter.text := FSetMain.ComPrinter;
   ckDuploClick.Checked:= iif(FSetMain.Duploclick<>0,1,0);
end;

procedure TfrmcfgImp.Button1Click(Sender: TObject);
begin
  FSetMain.ComPrinter:= edComPrinter.text;
  FSetMain.Duploclick:= iif(ckDuploClick.Checked,1,0);
  FSetMain.SalvaContexto(false);
  Close;
end;

procedure TfrmcfgImp.FormShow(Sender: TObject);
begin

end;

procedure TfrmcfgImp.Image5Click(Sender: TObject);
begin
  if (FSetMain.Duploclick = 0) then
  begin
    close();
  end;
end;

procedure TfrmcfgImp.Image5DblClick(Sender: TObject);
begin
  if (FSetMain.Duploclick <> 0) then
  begin
    close();
  end;
end;

end.

