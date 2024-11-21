unit ToolsOuvir;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ExtCtrls,
  lNetComponents, lNet, strutils, toolsfalar;

type

  { TfrmToolsOuvir }

  TfrmToolsOuvir = class(TForm)
    btConect: TButton;
    btDisconect: TButton;
    edIP: TEdit;
    edTexto: TEdit;
    edPort: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    LTCPComponent1: TLTCPComponent;
    LTCPComponent2: TLTCPComponent;
    Shape1: TShape;
    procedure btConectClick(Sender: TObject);
    procedure btDisconectClick(Sender: TObject);
    procedure LTCPComponent1Receive(aSocket: TLSocket);
    procedure LTCPComponent2Connect(aSocket: TLSocket);
    procedure LTCPComponent2Disconnect(aSocket: TLSocket);
    procedure LTCPComponent2Receive(aSocket: TLSocket);
    procedure Shape1ChangeBounds(Sender: TObject);
  private
    lastfrase : string;
  public
    frase : string;
    procedure Conectar();
    procedure Disconectar();
    procedure EnviaPerguntaAssociada(texto : String);

  end;

var
  frmToolsOuvir: TfrmToolsOuvir;

implementation

{$R *.lfm}

uses main;

{ TfrmToolsOuvir }


procedure TfrmToolsOuvir.Shape1ChangeBounds(Sender: TObject);
begin

end;

procedure TfrmToolsOuvir.btConectClick(Sender: TObject);
begin
   Conectar();
end;

procedure TfrmToolsOuvir.btDisconectClick(Sender: TObject);
begin
  Disconectar();
end;

procedure TfrmToolsOuvir.LTCPComponent1Receive(aSocket: TLSocket);
var
   info : String;
   posicao : integer;
begin
  //ShowMessage('Recebeu a mensagem:');
  info := '';
  aSocket.GetMessage(info);
  frase := 'doctor';
  posicao := pos(lowercase(frase),lowercase(info));
  edTexto.Text := lowercase(info);
  if(posicao<>0) then
  begin
       if(lastfrase <> info) then
       begin
         lastfrase := info;
         info := replacestr(info,frase, '');
         frmToolsfalar.falar('Claro, deixa eu ver se encontro sua resposta!');
         //frmmain.NewContext();
         //frmmain.pergunta := info;

         //frmmain.FazPergunta();
         //Registra a frase que veio
         frmmain.RecebeuPergunta(info);

         //Solicita verificação da associação do texto ao servico python
         EnviaPerguntaAssociada(info);
       end;


  end;

  //ShowMessage(info);
end;

procedure TfrmToolsOuvir.LTCPComponent2Connect(aSocket: TLSocket);
begin

end;

procedure TfrmToolsOuvir.LTCPComponent2Disconnect(aSocket: TLSocket);
begin

end;

procedure TfrmToolsOuvir.LTCPComponent2Receive(aSocket: TLSocket);
var
   info : String;
   posicao : integer;
begin
  //ShowMessage('Recebeu a mensagem:');
  info := '';
  aSocket.GetMessage(info);


  //edTexto.Text := lowercase(info);
  if(info<>'-1') then
  begin
    frmToolsfalar.Falar('Encontrei uma resposta');
    frmmain.RecebeuAssociacao(info );
  end;

  //ShowMessage(info);
end;

procedure TfrmToolsOuvir.Conectar();
begin
  LTCPComponent1.Connect(edIP.text,strtoint(edPort.text));
  LTCPComponent2.Connect(edIP.text,strtoint(edPort.text)+1);
  if(not LTCPComponent1.Active) then
  begin
       frmToolsfalar.Falar('Não foi possível conectar no serviço de escutar voz');
  end;
  if(not LTCPComponent2.Active) then
  begin
       frmToolsfalar.Falar('Não foi possível conectar no serviço de processar voz');
  end;

end;

procedure TfrmToolsOuvir.Disconectar();
begin
  LTCPComponent1.Disconnect(true);
  LTCPComponent2.Disconnect(true);
end;

procedure TfrmToolsOuvir.EnviaPerguntaAssociada(texto: String);
begin
 LTCPComponent2.SendMessage(texto, nil);
end;

end.

