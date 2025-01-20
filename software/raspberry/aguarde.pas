unit aguarde;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ExtCtrls,
  GifAnim, Toolsfalar, banco;

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
  if(dmbanco.flgStart) then
  begin
    close;
  end;
end;

procedure TfrmAguarde.FormCreate(Sender: TObject);
begin
  {$IFDEF LINUX}
    GifAnim1.FileName := '/home/mmm/projetos/maurinsoft/doctor/imgs/inicializando.gif';
  {$ENDIF}
  {$IFDEF WINDOWS}
    GifAnim1.FileName := 'D:\projetos\maurinsoft\doctor\imgs\inicializando.gif';
  {$ENDIF}
  frmToolsfalar.Falar('Estou Inicializando o equipamento, aguarde! '); //Ola
  if(frmToolsfalar<> nil) then
  begin

  end;
end;

end.

