unit cfgarduino;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ExtCtrls,
  setmain;

type

  { Tfrmcfgarduino }

  Tfrmcfgarduino = class(TForm)
    Button1: TButton;
    edComArduino: TEdit;
    Image5: TImage;
    Label1: TLabel;
    Label5: TLabel;
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure Image5Click(Sender: TObject);
    procedure Image5DblClick(Sender: TObject);
  private

  public

  end;

var
  frmcfgarduino: Tfrmcfgarduino;

implementation

{$R *.lfm}

{ Tfrmcfgarduino }

procedure Tfrmcfgarduino.FormCreate(Sender: TObject);
begin
  edComArduino.text := FSetMain.ComArduino;

end;

procedure Tfrmcfgarduino.Image5Click(Sender: TObject);
begin
  if (FSetMain.Duploclick = 0) then
  begin
    close();
  end;
end;

procedure Tfrmcfgarduino.Image5DblClick(Sender: TObject);
begin
  if (FSetMain.Duploclick <> 0) then
  begin
    close();
  end;
end;

procedure Tfrmcfgarduino.Button1Click(Sender: TObject);
begin
  FSetMain.ComArduino:= edComArduino.text;
  FSetMain.SalvaContexto(false);
  Close;
end;

end.

