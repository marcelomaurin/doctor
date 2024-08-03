unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  setmain, Toolsfalar, config, banco, bloqueio, OpAmostragem, imp, LazSerial,
  ToolsOuvir, Etiquetar;

type

  { Tfrmmain }

  Tfrmmain = class(TForm)
    Image1: TImage;
    Image2: TImage;
    Image3: TImage;
    Image4: TImage;
    Image5: TImage;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    LazSerial1: TLazSerial;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Image1Click(Sender: TObject);
    procedure Image1DblClick(Sender: TObject);
    procedure Image1MouseEnter(Sender: TObject);
    procedure Image2Click(Sender: TObject);
    procedure Image2DblClick(Sender: TObject);
    procedure Image2MouseEnter(Sender: TObject);
    procedure Image3MouseEnter(Sender: TObject);
    procedure Image4MouseEnter(Sender: TObject);
    procedure Image5Click(Sender: TObject);
    procedure Image5DblClick(Sender: TObject);
    procedure Image5MouseEnter(Sender: TObject);
  private
    procedure Setup();
    procedure ChamaConfiguracao();
    procedure ChamaAmostragem();

  public
    chavepessoa : string;
    nomepessoa : string;
    idpessoa : integer;

    procedure BloqueioAcesso();
    procedure RecebeuPergunta(Pergunta: string);
    procedure RecebeuAssociacao(Associacao : string);
    procedure StatusSistema();
    procedure SairSistema();
    procedure Etiquetagem();

  end;

var
  frmmain: Tfrmmain;

implementation

{$R *.lfm}

{ Tfrmmain }

procedure Tfrmmain.FormCreate(Sender: TObject);
begin

  Setup();
end;

procedure Tfrmmain.FormDestroy(Sender: TObject);
begin
  FSetMain.SalvaContexto(true);
  frmToolsfalar.Disconectar();
  frmToolsfalar.free;
end;

procedure Tfrmmain.Image1Click(Sender: TObject);
begin
  if(FSetMain.Duploclick=0) then
  begin
     ChamaConfiguracao();
  end;

end;

procedure Tfrmmain.Image1DblClick(Sender: TObject);
begin
  if(FSetMain.Duploclick<>0) then
  begin
     ChamaConfiguracao();
  end;
end;

procedure Tfrmmain.Image1MouseEnter(Sender: TObject);
begin
  frmToolsfalar.Falar('Configurações');
  Application.ProcessMessages;
end;

procedure Tfrmmain.Image2Click(Sender: TObject);
begin
  if(FSetMain.Duploclick=0) then
  begin
       ChamaAmostragem();
  end;
end;

procedure Tfrmmain.Image2DblClick(Sender: TObject);
begin
  if(FSetMain.Duploclick<>0) then
  begin
       ChamaAmostragem();
  end;
end;

procedure Tfrmmain.Image2MouseEnter(Sender: TObject);
begin
  frmToolsfalar.Falar('Amostragem');
  Application.ProcessMessages;
end;

procedure Tfrmmain.Image3MouseEnter(Sender: TObject);
begin
  frmToolsfalar.Falar('Análise');
  Application.ProcessMessages;
end;

procedure Tfrmmain.Image4MouseEnter(Sender: TObject);
begin
  frmToolsfalar.Falar('Relatórios');
  Application.ProcessMessages;
end;

procedure Tfrmmain.Image5Click(Sender: TObject);
begin
  if(FSetMain.Duploclick=0) then
  begin
    BloqueioAcesso();

  end;
end;

procedure Tfrmmain.Image5DblClick(Sender: TObject);
begin
  if(FSetMain.Duploclick<>0) then
  begin
    BloqueioAcesso();
  end;

end;

procedure Tfrmmain.Image5MouseEnter(Sender: TObject);
begin
  frmToolsfalar.Falar('Bloqueio de acesso');
  Application.ProcessMessages;
end;

procedure Tfrmmain.Setup;
begin
  FSetMain := TSetMain.create();
  FSetMain.CarregaContexto();

  dmbanco := Tdmbanco.Create(self);
  Application.ProcessMessages;
  if dmbanco.ZConnection1.Connected then
  begin
    frmToolsfalar :=   TfrmToolsfalar.create(self);
    frmToolsOuvir := TfrmToolsOuvir.create(self);
    Application.ProcessMessages;
    frmToolsfalar.Show;
    frmToolsOuvir.Show;
    Application.ProcessMessages;

    frmToolsfalar.edIP.text :=  Fsetmain.IPFALAR;
    frmToolsOuvir.edIP.text :=  Fsetmain.IPFALAR;
    Application.ProcessMessages;
    Sleep(1000);

    frmToolsfalar.Conectar();
    frmToolsOuvir.Conectar();
    Application.ProcessMessages;



    frmToolsfalar.Falar('Iniciando o servidor do Doctor, aguarde');
    Application.ProcessMessages;
    LazSerial1.Device:= FSetMain.ComPrinter;
    LazSerial1.BaudRate:= br__9600;
    LazSerial1.DataBits:=db8bits;
    LazSerial1.FlowControl := fcNone;
    LazSerial1.StopBits:= sbOne;
    LazSerial1.Open;
    if( not LazSerial1.Active) then
    begin
      frmToolsfalar.Falar('Atenção, houve um problema na porta serial, reconfigure a porta e reinicie a aplicação');
      Application.ProcessMessages;

      //Application.Terminate;

    end;
    Fimp := TImp.create(LazSerial1);
    Sleep(10000);
    //frmToolsfalar.Hide;
    //Application.ProcessMessages;
    //Finalizando o Setup
    frmToolsfalar.Falar('Equipamento pronto para operar');
    Application.ProcessMessages;
    BloqueioAcesso();
  end
  else
  begin
    frmToolsfalar.Falar('Atenção, houve um problema na conexão com o banco de dados, mude as configurações do servidor');
    Application.ProcessMessages;

    Application.Terminate;

  end;
end;

procedure Tfrmmain.ChamaConfiguracao;
begin
  frmconfig :=  Tfrmconfig.create(self);
  frmToolsfalar.Falar('Configurações do Sistema');
  Application.ProcessMessages;
  frmconfig.showmodal();
  frmconfig.free();
  frmToolsfalar.Falar('Voltou a Tela Principal');
  Application.ProcessMessages;
end;

procedure Tfrmmain.ChamaAmostragem;
begin
  frmOpAmostragem :=  TfrmOpAmostragem.create(self);
  frmToolsfalar.Falar('Operações de Amostragem');
  Application.ProcessMessages;
  frmOpAmostragem.showmodal();
  frmOpAmostragem.free();
  frmToolsfalar.Falar('Voltou a Tela Principal');
  Application.ProcessMessages;
end;

procedure Tfrmmain.BloqueioAcesso;
begin
    frmBloqueio := TfrmBloqueio.create(self);
    frmBloqueio.showmodal();
    chavepessoa :=frmBloqueio.edChave.text;
    nomepessoa := dmbanco.BuscaNomePessoa(chavepessoa);
    idpessoa:= dmbanco.BuscaIDPessoa(chavepessoa);
    frmToolsfalar.Falar('Bem vindo ao Sistema '+ nomepessoa);
    Application.ProcessMessages;
    frmBloqueio.free;

end;

procedure Tfrmmain.RecebeuPergunta(Pergunta: string);
begin
  dmbanco.RegistraOuve(Pergunta);

end;

procedure Tfrmmain.RecebeuAssociacao(Associacao: string);
var
  comando: integer;
begin
  //Recebeu comando
  comando := dmbanco.RetornaComando(strtoint(Associacao));

  case comando of
    1: frmToolsfalar.Falar('Olá, Estou pronto para te atender. Em que posso ajuda-lo? '); //Ola
    2: StatusSistema();
    3: SairSistema();
    4: Etiquetagem();
  else
        frmToolsfalar.Falar('Comando não encontrado '); //Comando nao encontrado
  end;

end;

procedure Tfrmmain.StatusSistema;
begin
  frmToolsfalar.Falar('Tudo ok com o sistema '); //Status do equipamento
end;

procedure Tfrmmain.SairSistema;
begin
  frmToolsfalar.Falar('Estou finalizando a aplicação, obrigado'); //Status do equipamento
  application.Terminate;
end;

procedure Tfrmmain.Etiquetagem;
begin
  frmEtiquetar := TfrmEtiquetar.create(self);
  frmEtiquetar.showmodal();
  frmEtiquetar.free();
end;

end.

