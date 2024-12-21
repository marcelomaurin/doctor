unit splash;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, GifAnim;

type

  { Tfrmsplash }

  Tfrmsplash = class(TForm)
    GifAnim1: TGifAnim;
    Timer1: TTimer;
    procedure FormShow(Sender: TObject);
    procedure GifAnim1StopAnim(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
  private

  public

  end;

var
  frmsplash: Tfrmsplash;

implementation

{$R *.lfm}

{ Tfrmsplash }

procedure Tfrmsplash.FormShow(Sender: TObject);
begin
  GifAnim1.Animate:= true;
  Timer1.Enabled:= true;
end;

procedure Tfrmsplash.GifAnim1StopAnim(Sender: TObject);
begin
  //showmessage('terminou');
  sleep(1000);

end;

procedure Tfrmsplash.Timer1Timer(Sender: TObject);
begin
  close;
end;

end.

