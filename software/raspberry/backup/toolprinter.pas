unit toolprinter;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ExtCtrls,
  lNetComponents, AdvLed, lNet;

type

  { Tfrmtoolprinter }

  Tfrmtoolprinter = class(TForm)
    AdvLed1: TAdvLed;
    btConect: TButton;
    btDisconect: TButton;
    btcmd: TButton;
    edcmd: TEdit;
    edIP: TEdit;
    edPort: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    LTCPComponent1: TLTCPComponent;
    Shape1: TShape;
    procedure btcmdClick(Sender: TObject);
    procedure btConectClick(Sender: TObject);
    procedure btDisconectClick(Sender: TObject);
    procedure LTCPComponent1Connect(aSocket: TLSocket);
    procedure LTCPComponent1Disconnect(aSocket: TLSocket);
    procedure LTCPComponent1Receive(aSocket: TLSocket);
  private

  public
    procedure Conectar();
    procedure Disconectar();
    procedure CallCMD();
    procedure CallCMD(Texto: string);

  end;

var
  frmtoolprinter: Tfrmtoolprinter;

implementation

{$R *.lfm}

{ Tfrmtoolprinter }

procedure Tfrmtoolprinter.LTCPComponent1Receive(aSocket: TLSocket);
var
   info : String;
begin
  aSocket.GetMessage(info);
end;

procedure Tfrmtoolprinter.LTCPComponent1Connect(aSocket: TLSocket);
begin
  //ShowMessage('Conectou!');
  AdvLed1.Blink:=false;
  AdvLed1.State:= lsOn;
end;

procedure Tfrmtoolprinter.btDisconectClick(Sender: TObject);
begin
  Disconectar();
end;

procedure Tfrmtoolprinter.btConectClick(Sender: TObject);
begin
  Conectar();
end;

procedure Tfrmtoolprinter.btcmdClick(Sender: TObject);
begin
  (*
  if(Fsetmain.IPFALAR <> edIP.text) then
  begin
    Fsetmain.IPFALAR := edIP.text ;
    Fsetmain.SalvaContexto(false);
  end;
  *)
  CallCMD();
end;

procedure Tfrmtoolprinter.LTCPComponent1Disconnect(aSocket: TLSocket);
begin
  //ShowMessage('Disconectou');
  AdvLed1.Blink:=false;
  AdvLed1.State:= lsOff;
end;

procedure Tfrmtoolprinter.Conectar();
begin
    LTCPComponent1.Connect(edIP.text,strtoint(edPort.text));
end;

procedure Tfrmtoolprinter.Disconectar();
begin
   LTCPComponent1.Disconnect(true);
end;

procedure Tfrmtoolprinter.CallCMD();
var
   pergunta: string;
begin
  pergunta := edcmd.text+#13;
  if(pergunta<>'') then
  begin
     LTCPComponent1.SendMessage(pergunta,nil);
     //Disconectar();
  end;
end;

procedure Tfrmtoolprinter.CallCMD(Texto: string);
begin
  edCMD.text := texto;
  CallCMD();
end;

end.

