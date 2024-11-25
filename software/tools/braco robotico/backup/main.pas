unit Main; 

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, StdCtrls,
  ComCtrls, ExtCtrls, SdpoSerial, SdpoJoystick;

type

  { Tfrmmain }

  Tfrmmain = class(TForm)
    Button1: TButton;
    Button10: TButton;
    Button11: TButton;
    Button12: TButton;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    Button5: TButton;
    Button6: TButton;
    Button7: TButton;
    Button8: TButton;
    Button9: TButton;
    ckJoystick: TCheckBox;
    Edit1: TEdit;
    edPorta: TEdit;
    Image1: TImage;
    Image2: TImage;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    Label7: TLabel;
    Label8: TLabel;
    lstMov: TListBox;
    OpenDialog1: TOpenDialog;
    PageControl1: TPageControl;
    SaveDialog1: TSaveDialog;
    SdpoJoystick1: TSdpoJoystick;
    SdpoSerial1: TSdpoSerial;
    Shape1: TShape;
    Shape2: TShape;
    Shape3: TShape;
    TabSheet1: TTabSheet;
    TabSheet2: TTabSheet;
    TabSheet3: TTabSheet;
    tbforte: TTrackBar;
    tbfraco: TTrackBar;
    tbposicao: TTrackBar;
    procedure Button10Click(Sender: TObject);
    procedure Button11Click(Sender: TObject);
    procedure Button12Click(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    procedure Button6Click(Sender: TObject);
    procedure Button7Click(Sender: TObject);
    procedure Button8Click(Sender: TObject);
    procedure Button9Click(Sender: TObject);
    procedure Edit1KeyPress(Sender: TObject; var Key: char);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    { private declarations }
    operador : integer;
    posicao : integer;
    procedure setservo(ope : integer; referencia : integer);
    procedure moverigth(forca : integer);
    procedure moveleft(forca : integer);

  public
    { public declarations }

  end; 

var
  frmmain: Tfrmmain;

implementation

{$R *.lfm}

{ Tfrmmain }

//Define o operador correto
procedure Tfrmmain.setservo( ope : integer; referencia : integer);
begin
  operador := ope;
  posicao := referencia;
  lstMov.Items.Add('SERVOSEL='+inttostr(ope));
end;

procedure Tfrmmain.moverigth(forca : integer);
begin

  Posicao:= posicao -forca;
  if (posicao < 0) then posicao := 0;
  SdpoSerial1.WriteData(chr(operador+ord('0'))+chr(posicao+ord(0))+#13);
  tbposicao.Position:= posicao;
  lstMov.Items.Add('SERVOMOV='+inttostr(01+30));
end;

procedure Tfrmmain.moveleft(forca : integer);
begin
  Posicao:= trunc(posicao +forca);
  if (posicao > 255) then posicao := 255;
  SdpoSerial1.WriteData(chr(operador+ord('0'))+chr(posicao+ord('0'))+#13);
  tbposicao.Position:= posicao;
  lstMov.Items.Add('MOVLef:'+IntToStr(255));
end;


procedure Tfrmmain.Button1Click(Sender: TObject);
begin
  SdpoSerial1.WriteData('10'+#13);

end;

procedure Tfrmmain.Button10Click(Sender: TObject);
begin
  lstMov.Items.clear;;
end;

procedure Tfrmmain.Button11Click(Sender: TObject);
begin
  if SaveDialog1.Execute then
  begin
       lstMov.Items.SaveToFile(SaveDialog1.FileName);
       ShowMessage('Arquivo Salvo');
  end;
end;

procedure Tfrmmain.Button12Click(Sender: TObject);
begin
  if OpenDialog1.Execute then
  begin
       Button10Click(self); //limpa movimentos
       lstMov.Items.LoadFromFile(OpenDialog1.FileName);
  end;
end;

procedure Tfrmmain.Button2Click(Sender: TObject);
begin
  moveleft(tbforte.Position);
end;

procedure Tfrmmain.Button3Click(Sender: TObject);
begin
  moverigth(tbforte.Position);
end;

procedure Tfrmmain.Button4Click(Sender: TObject);
begin
    moverigth(tbfraco.Position);
end;

procedure Tfrmmain.Button5Click(Sender: TObject);
begin
  moveleft(tbfraco.Position);
end;

procedure Tfrmmain.Button6Click(Sender: TObject);
begin
  setservo(2, 44);
end;

procedure Tfrmmain.Button7Click(Sender: TObject);
begin
  setservo(3, 215);
end;

procedure Tfrmmain.Button8Click(Sender: TObject);
begin
  setservo(1, 214);
end;

procedure Tfrmmain.Button9Click(Sender: TObject);
begin
  setservo(4, 60);
end;

procedure Tfrmmain.Edit1KeyPress(Sender: TObject; var Key: char);
begin
  if (key = '4') then
  begin
    moverigth(tbforte.Position);
  end;
  if (key = '6') then
  begin
    moveleft(tbforte.Position);
  end;
  if (key = 'a') then
  begin
    setservo(1, 60);
  end;
  if (key = 's') then
  begin
    setservo(2,60);
  end;
    if (key = 'd') then
  begin
    setservo(3, 60);
  end;
  if (key = 'f') then
  begin
      setservo(4, 60);
  end;
  if (key = 'g') then
  begin
    setservo(5, 60);
  end;
  if (key = 'h') then
  begin
    setservo(6, 60);
  end;
  if (key = 'j') then
  begin
    setservo(7, 60);
  end;
  if (key = 'k') then
  begin
    setservo(8, 60);
  end;
end;

procedure Tfrmmain.FormCreate(Sender: TObject);
begin
  operador := 1;
  SdpoSerial1.close;
  SdpoSerial1.Device :=  edPorta.text;
  SdpoSerial1.Open;

end;

procedure Tfrmmain.FormDestroy(Sender: TObject);
begin
    SdpoSerial1.Close;

end;

end.

