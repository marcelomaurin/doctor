unit cfgImp;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, banco, imp,
  setmain, funcoes;

type

  { TfrmcfgImp }

  TfrmcfgImp = class(TForm)
    Button1: TButton;
    ckDuploClick: TCheckBox;
    edComPrinter: TEdit;
    Label1: TLabel;
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
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

end.

