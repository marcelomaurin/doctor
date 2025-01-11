unit aguarde;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ExtCtrls,
  GifAnim;

type

  { TfrmAguarde }

  TfrmAguarde = class(TForm)
    GifAnim1: TGifAnim;
    Label1: TLabel;
    Timer1: TTimer;
    procedure Timer1Timer(Sender: TObject);
  private

  public

  end;

var
  frmAguarde: TfrmAguarde;

implementation

{$R *.lfm}

uses main;

{ TfrmAguarde }

procedure TfrmAguarde.Timer1Timer(Sender: TObject);
begin
  if(frmmain.flgStart) then
  begin
    close;
  end;
end;

end.

