unit brobotico; 

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, StdCtrls,
  ComCtrls, ExtCtrls, SdpoSerial, synaser, setmain;

type

  { Tfrmbrobotico }

  Tfrmbrobotico = class(TForm)
    btTestar: TButton;
    Button10: TButton;
    Button11: TButton;
    Button12: TButton;
    Button13: TButton;
    Button14: TButton;
    btDirForte: TButton;
    btEsqForte: TButton;
    btEsqFraco: TButton;
    btDirFraco: TButton;
    btBraco: TButton;
    btPunho: TButton;
    btGirar: TButton;
    btGarra: TButton;
    Edit1: TEdit;
    edPorta: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    lstMov: TListBox;
    meconsole: TMemo;
    OpenDialog1: TOpenDialog;
    PageControl1: TPageControl;
    SaveDialog1: TSaveDialog;
    SdpoSerial1: TSdpoSerial;
    Shape1: TShape;
    Shape2: TShape;
    Shape3: TShape;
    tsconsole: TTabSheet;
    TabSheet2: TTabSheet;
    TabSheet3: TTabSheet;
    tbforte: TTrackBar;
    tbfraco: TTrackBar;
    tbposicao: TTrackBar;
    procedure Button10Click(Sender: TObject);
    procedure Button11Click(Sender: TObject);
    procedure Button12Click(Sender: TObject);
    procedure Button13Click(Sender: TObject);
    procedure Button14Click(Sender: TObject);
    procedure btTestarClick(Sender: TObject);
    procedure btDirForteClick(Sender: TObject);
    procedure btEsqForteClick(Sender: TObject);
    procedure btEsqFracoClick(Sender: TObject);
    procedure btDirFracoClick(Sender: TObject);
    procedure btBracoClick(Sender: TObject);
    procedure btPunhoClick(Sender: TObject);
    procedure btGirarClick(Sender: TObject);
    procedure btGarraClick(Sender: TObject);
    procedure Edit1KeyPress(Sender: TObject; var Key: char);
    procedure edPortaKeyPress(Sender: TObject; var Key: char);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure SdpoSerial1BlockSerialStatus(Sender: TObject;
      Reason: THookSerialReason; const Value: string);
    procedure SdpoSerial1RxData(Sender: TObject);
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
  frmbrobotico: Tfrmbrobotico;

implementation

{$R *.lfm}

{ Tfrmbrobotico }

//Define o operador correto
procedure Tfrmbrobotico.setservo( ope : integer; referencia : integer);
begin
  operador := ope;
  posicao := referencia;
  lstMov.Items.Add('MOVESERVO='+inttostr(ope)+','+inttostr(referencia));
end;

procedure Tfrmbrobotico.moverigth(forca : integer);
begin

  Posicao:= posicao -forca;
  if (posicao < 0) then posicao := 0;
  SdpoSerial1.WriteData(chr(operador)+chr(posicao));
  tbposicao.Position:= posicao;
  lstMov.Items.Add('MOVEDIR='+inttostr(01+30));
end;

procedure Tfrmbrobotico.moveleft(forca : integer);
begin
  Posicao:= trunc(posicao +forca);
  if (posicao > 255) then posicao := 255;
  SdpoSerial1.WriteData(chr(operador)+chr(posicao));
  tbposicao.Position:= posicao;
  lstMov.Items.Add('MOVEESQ='+IntToStr(255));
end;


procedure Tfrmbrobotico.btTestarClick(Sender: TObject);
begin
  SdpoSerial1.WriteData(#01+#01);

end;

procedure Tfrmbrobotico.Button10Click(Sender: TObject);
begin
  lstMov.Items.clear;;
end;

procedure Tfrmbrobotico.Button11Click(Sender: TObject);
begin
  if SaveDialog1.Execute then
  begin
       lstMov.Items.SaveToFile(SaveDialog1.FileName);
       ShowMessage('Arquivo Salvo');
  end;
end;

procedure Tfrmbrobotico.Button12Click(Sender: TObject);
begin
  if OpenDialog1.Execute then
  begin
       Button10Click(self); //limpa movimentos
       lstMov.Items.LoadFromFile(OpenDialog1.FileName);
  end;
end;

procedure Tfrmbrobotico.Button13Click(Sender: TObject);
begin
  SdpoSerial1.close;
  SdpoSerial1.Device :=  edPorta.text;
  fsetmain.SerialPort :=  edPorta.text;
  FSetMain.SalvaContexto(false);
  SdpoSerial1.Open;
  PageControl1.PageIndex := 0;
  Application.ProcessMessages;

end;

procedure Tfrmbrobotico.Button14Click(Sender: TObject);
begin
  SdpoSerial1.close;
  SdpoSerial1.Device :=  edPorta.text;

end;

procedure Tfrmbrobotico.btDirForteClick(Sender: TObject);
begin
  moveleft(tbforte.Position);
end;

procedure Tfrmbrobotico.btEsqForteClick(Sender: TObject);
begin
  moverigth(tbforte.Position);
end;

procedure Tfrmbrobotico.btEsqFracoClick(Sender: TObject);
begin
    moverigth(tbfraco.Position);
end;

procedure Tfrmbrobotico.btDirFracoClick(Sender: TObject);
begin
  moveleft(tbfraco.Position);
end;

procedure Tfrmbrobotico.btBracoClick(Sender: TObject);
begin
  setservo(2, tbposicao.Position);
end;

procedure Tfrmbrobotico.btPunhoClick(Sender: TObject);
begin
  setservo(3, 215);
end;

procedure Tfrmbrobotico.btGirarClick(Sender: TObject);
begin
  setservo(1, tbposicao.Position);
end;

procedure Tfrmbrobotico.btGarraClick(Sender: TObject);
begin
  setservo(4, tbposicao.Position);
end;

procedure Tfrmbrobotico.Edit1KeyPress(Sender: TObject; var Key: char);
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

procedure Tfrmbrobotico.edPortaKeyPress(Sender: TObject; var Key: char);
begin
   if(key = #13) then
   begin
       FSetMain.SerialPort := edPorta.text;
   end;
end;

procedure Tfrmbrobotico.FormCreate(Sender: TObject);
begin
  operador := 1;
  edPorta.text := FSetMain.SerialPort ;
  PageControl1.ActivePage := TabSheet3;


end;

procedure Tfrmbrobotico.FormDestroy(Sender: TObject);
begin
    SdpoSerial1.Close;

end;

procedure Tfrmbrobotico.SdpoSerial1BlockSerialStatus(Sender: TObject;
  Reason: THookSerialReason; const Value: string);
begin

end;

procedure Tfrmbrobotico.SdpoSerial1RxData(Sender: TObject);
begin
  if SdpoSerial1.DataAvailable then
  begin
    meconsole.Append(SdpoSerial1.ReadData);
  end;
end;

end.

