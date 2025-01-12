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
    procedure FormCreate(Sender: TObject);
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

procedure TfrmAguarde.FormCreate(Sender: TObject);
begin
  {$IFDEF LINUX}
    GifAnim1.FileName := 'D:\projetos\maurinsoft\doctor\imgs\inicializando.gif';
  {$ENDIF}
  {$IFDEF WINDOWS}
    GifAnim1.FileName := 'D:\projetos\maurinsoft\doctor\imgs\inicializando.gif';
  {$ENDIF}
end;

end.

