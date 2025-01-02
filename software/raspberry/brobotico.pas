unit brobotico; 

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, StdCtrls,
  ComCtrls, ExtCtrls, synaser, setmain, LazSerial, MKnob;

type

  { Tfrmbrobotico }

  Tfrmbrobotico = class(TForm)
    btTestar: TButton;
    btTestar1: TButton;
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
    edPorta: TEdit;
    Image1: TImage;
    Image2: TImage;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    lbPosicao: TLabel;
    lbPosicao1: TLabel;
    lstMov: TListBox;
    meconsole: TMemo;
    mKnob1: TmKnob;
    OpenDialog1: TOpenDialog;
    PageControl1: TPageControl;
    SaveDialog1: TSaveDialog;
    Shape1: TShape;
    Shape3: TShape;
    TabSheet1: TTabSheet;
    TabSheet4: TTabSheet;
    tbMov: TTrackBar;
    tbposicao1: TTrackBar;
    tbposicao2: TTrackBar;
    tsconsole: TTabSheet;
    TabSheet2: TTabSheet;
    TabSheet3: TTabSheet;
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
    procedure tbMovChange(Sender: TObject);
    procedure tbposicaoChange(Sender: TObject);
  private
    { private declarations }
    operador : integer;
    posicao : integer;
    procedure setservo(ope : integer; referencia : integer);
    procedure moverigth(forca : integer);
    procedure moveleft(forca : integer);

  public
    { public declarations }
    posicaoanterior : integer;
  end; 

var
  frmbrobotico: Tfrmbrobotico;

implementation

{$R *.lfm}

{ Tfrmbrobotico }

uses main;

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
  frmmain.LazSerial2.WriteData('MOVESERVO='+inttostr(operador)+','+inttostr(posicao)+#13+#10);
  tbposicao.Position:= posicao;
  lstMov.Items.Add('MOVESERVO='+inttostr(operador)+','+inttostr(posicao));
end;

procedure Tfrmbrobotico.moveleft(forca : integer);
begin
  Posicao:= trunc(posicao +forca);
  if (posicao > 255) then posicao := 255;
  frmmain.LazSerial2.WriteData('MOVESERVO='+inttostr(operador)+','+inttostr(posicao)+#13+#10);
  tbposicao.Position:= posicao;
  lstMov.Items.Add('MOVESERVO='+inttostr(operador)+','+inttostr(posicao));
end;


procedure Tfrmbrobotico.btTestarClick(Sender: TObject);
begin
  frmmain.LazSerial2.WriteData('RETORNAR'+#13+#10);

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
  frmmain.LazSerial2.close;
  frmmain.LazSerial2.Device :=  edPorta.text;
  fsetmain.SerialPort :=  edPorta.text;
  FSetMain.SalvaContexto(false);
  frmmain.LazSerial2.Open;
  PageControl1.PageIndex := 0;
  Application.ProcessMessages;

end;

procedure Tfrmbrobotico.Button14Click(Sender: TObject);
begin
  frmmain.LazSerial2.close;
  frmmain.LazSerial2.Device :=  edPorta.text;

end;

procedure Tfrmbrobotico.btDirForteClick(Sender: TObject);
begin
  //moveleft(tbforte.Position);
end;

procedure Tfrmbrobotico.btEsqForteClick(Sender: TObject);
begin
  //moverigth(tbforte.Position);
end;

procedure Tfrmbrobotico.btEsqFracoClick(Sender: TObject);
begin
    //moverigth(tbfraco.Position);
end;

procedure Tfrmbrobotico.btDirFracoClick(Sender: TObject);
begin
  //moveleft(tbfraco.Position);
end;

procedure Tfrmbrobotico.btBracoClick(Sender: TObject);
begin
  setservo(2, tbposicao.Position);
end;

procedure Tfrmbrobotico.btPunhoClick(Sender: TObject);
begin
  setservo(3, tbposicao.Position);
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
  posicaoanterior := 0;
  edPorta.text := FSetMain.SerialPort ;
  PageControl1.ActivePage := TabSheet3;


end;

procedure Tfrmbrobotico.FormDestroy(Sender: TObject);
begin
    //frmmain.LazSerial2.Close;

end;




procedure Tfrmbrobotico.tbMovChange(Sender: TObject);
begin
  if ((tbMov.Position- posicaoanterior) > 0) then
  begin
     frmmain.LazSerial2.WriteData('MOVEDIR='+inttostr(tbMov.Position-posicaoanterior)+#13+#10);
  end
  else
  begin
     frmmain.LazSerial2.WriteData('MOVEESQ='+inttostr(posicaoanterior-tbMov.Position)+#13+#10);
  end;
  posicaoanterior :=  tbMov.Position;
end;

procedure Tfrmbrobotico.tbposicaoChange(Sender: TObject);
begin
  setservo(operador, tbposicao.Position);
end;

end.

