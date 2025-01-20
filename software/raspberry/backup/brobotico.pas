unit brobotico; 

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, StdCtrls,
  ComCtrls, ExtCtrls, synaser, setmain, LazSerial, MKnob, LedNumber,
  indGnouMeter, AdvLed, log, banco;

type

  { Tfrmbrobotico }

  Tfrmbrobotico = class(TForm)
    AdvLed1: TAdvLed;
    AdvLed2: TAdvLed;
    btInicioEsteira: TButton;
    btCalibrar: TButton;
    btFinalEsteira: TButton;
    btMover: TButton;
    btPosFimServa: TButton;
    btAtivaBraco: TButton;
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
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    lbPosicao: TLabel;
    lbPOSFIMESTEIRA: TLabel;
    LEDTEMP: TLEDNumber;
    LEDHUM: TLEDNumber;
    lstMov: TListBox;
    mKnob1: TmKnob;
    OpenDialog1: TOpenDialog;
    PageControl1: TPageControl;
    SaveDialog1: TSaveDialog;
    Shape1: TShape;
    Shape3: TShape;
    btdesativabraco: TToggleBox;
    tsEsteira: TTabSheet;
    tsMovimento: TTabSheet;
    tsMonitor: TTabSheet;
    tbMov: TTrackBar;
    tbposicao1: TTrackBar;
    tbposicao2: TTrackBar;
    tsBraco: TTabSheet;
    tsConfig: TTabSheet;
    tbposicao: TTrackBar;
    procedure btAtivaBracoClick(Sender: TObject);
    procedure btCalibrarClick(Sender: TObject);
    procedure btdesativabracoChange(Sender: TObject);
    procedure btFinalEsteiraClick(Sender: TObject);
    procedure btMoverClick(Sender: TObject);
    procedure btPosFimServaClick(Sender: TObject);
    procedure Button10Click(Sender: TObject);
    procedure Button11Click(Sender: TObject);
    procedure Button12Click(Sender: TObject);
    procedure Button13Click(Sender: TObject);
    procedure Button14Click(Sender: TObject);
    procedure btInicioEsteiraClick(Sender: TObject);
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
    procedure tbMovEndDrag(Sender, Target: TObject; X, Y: Integer);
    procedure tbMovExit(Sender: TObject);
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
  dmbanco.LazSerial2.WriteData('MOVESERVO='+inttostr(operador)+','+inttostr(referencia)+#10);

end;

procedure Tfrmbrobotico.moverigth(forca : integer);
begin

  Posicao:= posicao -forca;
  if (posicao < 0) then posicao := 0;
  //frmmain.LazSerial2.OnRxData:= @frmmain.LazSerial2RxData;
  dmbanco.LazSerial2.WriteData('MOVESERVO='+inttostr(operador)+','+inttostr(posicao)+#10);
  tbposicao.Position:= posicao;
  lstMov.Items.Add('MOVESERVO='+inttostr(operador)+','+inttostr(posicao));
end;

procedure Tfrmbrobotico.moveleft(forca : integer);
begin
  Posicao:= trunc(posicao +forca);
  if (posicao > 255) then posicao := 255;
  //dmbanco.LazSerial2.OnRxData:= @frmmain.LazSerial2RxData;
  dmbanco.LazSerial2.WriteData('MOVESERVO='+inttostr(operador)+','+inttostr(posicao)+#10);
  tbposicao.Position:= posicao;
  lstMov.Items.Add('MOVESERVO='+inttostr(operador)+','+inttostr(posicao));
end;


procedure Tfrmbrobotico.btInicioEsteiraClick(Sender: TObject);
begin
  //LazSerial2.OnRxData:= @frmmain.LazSerial2RxData;
  dmbanco.LazSerial2.WriteData('RETORNOCARRO'+#10);

end;

procedure Tfrmbrobotico.Button10Click(Sender: TObject);
begin
  lstMov.Items.clear;;
end;

procedure Tfrmbrobotico.btFinalEsteiraClick(Sender: TObject);
begin
  //frmmain.LazSerial2.OnRxData:= @frmmain.LazSerial2RxData;
  dmbanco.LazSerial2.WriteData('POSFIMCARRO'+#10);

end;

procedure Tfrmbrobotico.btMoverClick(Sender: TObject);
begin
  if (tbMov.Position<> posicaoanterior) then
  begin
     //dmbanco.LazSerial2.OnRxData:= @frmmain.LazSerial2RxData;
     dmbanco.LazSerial2.WriteData('MOVEPASSO='+inttostr(tbMov.Position)+#10);
     posicaoanterior :=  tbMov.Position;
     Image2.left := trunc(tbMov.Position*0.28);
     lbPosicao.Caption:= inttostr(tbMov.Position);

  end;
end;

procedure Tfrmbrobotico.btPosFimServaClick(Sender: TObject);
begin
  if(dmbanco.LazSerial2.Active) then
  begin
       dmbanco.LazSerial2.WriteData('POSFIMSERVA'+#10);
  end
  else
  begin
       //Nao ativo
  end;
end;

procedure Tfrmbrobotico.btCalibrarClick(Sender: TObject);
begin
  if(dmbanco.LazSerial2.Active) then
  begin
       //dmbanco.LazSerial2.OnRxData:= @frmmain.LazSerial2RxData;
       dmbanco.LazSerial2.WriteData('CALIBRACAO'+#10);
  end;
end;

procedure Tfrmbrobotico.btdesativabracoChange(Sender: TObject);
begin
  if(dmbanco.LazSerial2.Active) then
  begin
       //dmbanco.LazSerial2.OnRxData:= @frmmain.LazSerial2RxData;
       dmbanco.LazSerial2.WriteData('DESATIVABRACO'+#10);
  end;
end;

procedure Tfrmbrobotico.btAtivaBracoClick(Sender: TObject);
begin
  if(dmbanco.LazSerial2.Active) then
  begin
       //dmbanco.LazSerial2.OnRxData:= @frmmain.LazSerial2RxData;
       dmbanco.LazSerial2.WriteData('ATIVABRACO'+#10);
  end;
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
  if(not dmbanco.LazSerial2.Active) then
  begin
    dmbanco.LazSerial2.close;
    dmbanco.LazSerial2.Device :=  edPorta.text;
    fsetmain.SerialPort :=  edPorta.text;
    FSetMain.SalvaContexto(false);
    dmbanco.LazSerial2.Open;
    //dmbanco.LazSerial2.OnRxData:= @frmmain.LazSerial2RxData;
    PageControl1.PageIndex := 0;
    Application.ProcessMessages;
  end;
end;

procedure Tfrmbrobotico.Button14Click(Sender: TObject);
begin
  if(dmbanco.LazSerial2.Active) then
  begin
    dmbanco.LazSerial2.close;
    dmbanco.LazSerial2.Device :=  edPorta.text;
  end;
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
  setservo(3, tbposicao1.Position);
end;

procedure Tfrmbrobotico.btGirarClick(Sender: TObject);
begin
  //setservo(1, tbposicao.Position);
  setservo(1, mKnob1.Position);
end;

procedure Tfrmbrobotico.btGarraClick(Sender: TObject);
begin
  setservo(4, tbposicao2.Position);
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
  PageControl1.ActivePage := tsEsteira;
  if (frmlog = nil) then
  begin
      frmlog := tfrmlog.create(self);
  end;
  frmlog.show;
  if(dmbanco.POSFIMESTEIRA<>0) then
  begin
    tbMov.max := dmbanco.POSFIMESTEIRA;
    lbPOSFIMESTEIRA.Caption:= inttostr(dmbanco.POSFIMESTEIRA);
  end;

end;

procedure Tfrmbrobotico.FormDestroy(Sender: TObject);
begin
    //frmmain.LazSerial2.Close;

    if (frmlog <> nil) then
    begin
        frmlog.close;
        frmlog.free;
        frmlog := nil;
    end;

end;

procedure Tfrmbrobotico.tbMovEndDrag(Sender, Target: TObject; X, Y: Integer);
begin

end;




procedure Tfrmbrobotico.tbMovExit(Sender: TObject);
begin

end;



procedure Tfrmbrobotico.tbposicaoChange(Sender: TObject);
begin

end;

end.

