unit toolsfalar;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, lNetComponents, Forms, Controls, Graphics,
  Dialogs, StdCtrls, ExtCtrls, AdvLed, lNet, setmain;

type

  { TfrmToolsfalar }

  TfrmToolsfalar = class(TForm)
    AdvLed1: TAdvLed;
    btFalar: TButton;
    btConect: TButton;
    btDisconect: TButton;
    edIP: TEdit;
    edFalar: TEdit;
    edPort: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    LTCPComponent1: TLTCPComponent;
    Shape1: TShape;
    procedure btConectClick(Sender: TObject);
    procedure btDisconectClick(Sender: TObject);
    procedure btFalarClick(Sender: TObject);
    procedure edPortChange(Sender: TObject);
    procedure LTCPComponent1Accept(aSocket: TLSocket);
    procedure LTCPComponent1Connect(aSocket: TLSocket);
    procedure LTCPComponent1Disconnect(aSocket: TLSocket);
    procedure LTCPComponent1Error(const msg: string; aSocket: TLSocket);
    procedure LTCPComponent1Receive(aSocket: TLSocket);
  private
    { private declarations }
  public
    { public declarations }
    procedure Falar();
    procedure Falar( Texto : string);
    procedure Conectar();
    procedure Disconectar();
  end;

var
  frmToolsfalar: TfrmToolsfalar;

implementation

{$R *.lfm}

{ TfrmMain }

procedure TfrmToolsfalar.btConectClick(Sender: TObject);
begin
  Conectar();
end;

procedure TfrmToolsfalar.btDisconectClick(Sender: TObject);
begin
  Disconectar();
end;

procedure TfrmToolsfalar.btFalarClick(Sender: TObject);
begin
  if(Fsetmain.IPFALAR <> edIP.text) then
  begin
    Fsetmain.IPFALAR := edIP.text ;
    Fsetmain.SalvaContexto(false);
  end;
  Falar();
end;

procedure TfrmToolsfalar.edPortChange(Sender: TObject);
begin

end;

procedure TfrmToolsfalar.LTCPComponent1Accept(aSocket: TLSocket);
begin
  AdvLed1.Blink:=false;
  AdvLed1.State:= lsOn;

end;

procedure TfrmToolsfalar.LTCPComponent1Connect(aSocket: TLSocket);
begin
  //ShowMessage('Conectou!');
  AdvLed1.Blink:=false;
  AdvLed1.State:= lsOn;
end;

procedure TfrmToolsfalar.LTCPComponent1Disconnect(aSocket: TLSocket);
begin
  //ShowMessage('Disconectou');
  AdvLed1.Blink:=false;
  AdvLed1.State:= lsOff;

end;

procedure TfrmToolsfalar.LTCPComponent1Error(const msg: string; aSocket: TLSocket);
begin
  //ShowMessage('Erro ao conectar!');
end;

procedure TfrmToolsfalar.LTCPComponent1Receive(aSocket: TLSocket);
var
   info : String;
begin
  //ShowMessage('Recebeu a mensagem:');

  aSocket.GetMessage(info);
  //ShowMessage(info);
end;

procedure TfrmToolsfalar.Falar();
var
   pergunta: string;
begin
  pergunta := edFalar.text;
  if(pergunta<>'') then
  begin
     LTCPComponent1.SendMessage(edFalar.text,nil);
     //Disconectar();
  end;
end;

procedure TfrmToolsfalar.Falar(Texto: string);
begin
  edFalar.text := texto;
  Falar();
end;

procedure TfrmToolsfalar.Conectar();
begin
  LTCPComponent1.Connect(edIP.text,strtoint(edPort.text));
end;

procedure TfrmToolsfalar.Disconectar();
begin
  LTCPComponent1.Disconnect(true);
end;

end.

