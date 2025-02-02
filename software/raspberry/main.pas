unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  setmain, Toolsfalar, config, banco, bloqueio, OpAmostragem, imp, LazSerial,
  ToolsOuvir, Etiquetar, splash, brobotico, log, aguarde, toolprinter,
  SdpoSerial;

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
    procedure FormChangeBounds(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Image1Click(Sender: TObject);
    procedure Image1DblClick(Sender: TObject);
    procedure Image1MouseEnter(Sender: TObject);
    procedure Image2Click(Sender: TObject);
    procedure Image2DblClick(Sender: TObject);
    procedure Image2MouseEnter(Sender: TObject);
    procedure Image3Click(Sender: TObject);
    procedure Image3MouseEnter(Sender: TObject);
    procedure Image4Click(Sender: TObject);
    procedure Image4MouseEnter(Sender: TObject);
    procedure Image5Click(Sender: TObject);
    procedure Image5DblClick(Sender: TObject);
    procedure Image5MouseEnter(Sender: TObject);

    procedure LazSerial2RxData(Sender: TObject);

    procedure SdpoSerial1RxData(Sender: TObject);
  private
    procedure Setup();
    procedure ChamaConfiguracao();
    procedure ChamaAmostragem();

  public
    chavepessoa : string;
    nomepessoa : string;
    idpessoa : integer;


    procedure AguardeInicializar();
    procedure BloqueioAcesso();
    procedure RecebeuPergunta(Pergunta: string);


  end;

var
  frmmain: Tfrmmain;

implementation

{$R *.lfm}

{ Tfrmmain }

procedure Tfrmmain.FormCreate(Sender: TObject);
begin

  frmsplash := tfrmsplash.create(self);
  frmsplash.showmodal();
  Setup();
  frmsplash.free;


end;

procedure Tfrmmain.FormChangeBounds(Sender: TObject);
begin

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

procedure Tfrmmain.Image3Click(Sender: TObject);
begin

end;

procedure Tfrmmain.Image3MouseEnter(Sender: TObject);
begin
  frmToolsfalar.Falar('Análise');
  Application.ProcessMessages;
end;

procedure Tfrmmain.Image4Click(Sender: TObject);
begin

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


procedure Tfrmmain.LazSerial2RxData(Sender: TObject);
begin

end;




procedure Tfrmmain.SdpoSerial1RxData(Sender: TObject);
begin

end;

procedure Tfrmmain.Setup;
begin
  FSetMain := TSetMain.create();
  FSetMain.CarregaContexto();
  dmbanco := Tdmbanco.Create(self);
  sleep(1000);

  Application.ProcessMessages;
  try
    (*
    //Application.ProcessMessages;
    LazSerial1.Device:= FSetMain.ComPrinter;
    LazSerial1.BaudRate:= br__9600;
    LazSerial1.DataBits:=db8bits;
    LazSerial1.FlowControl := fcNone;
    LazSerial1.StopBits:= sbOne;
    LazSerial1.Open;

    Fimp := TImp.create(LazSerial1);
    if( not LazSerial1.Active) then
    begin
      frmToolsfalar.Falar('Atenção, houve um problema na porta serial, reconfigure a porta e reinicie a aplicação');
      Application.ProcessMessages;

      //Application.Terminate;

    end;
    *)
     Fimp := TImp.create();
  finally

      Application.ProcessMessages;
      frmToolsfalar :=   TfrmToolsfalar.create(self);
      frmToolsfalar.Show;
      frmlog := Tfrmlog.create(self); //Cria o log de eventos
      frmlog.show;
      Application.ProcessMessages;
      try
        dmbanco.LazSerial2.Device:= FSetMain.SerialPort;
        dmbanco.LazSerial2.open;
        AguardeInicializar();
        dmbanco.CalibrarModulo1();
      except
         frmbrobotico := Tfrmbrobotico.create(self);
         ShowMessage('Necessário configurar porta do equipamento');
         frmbrobotico.show();
         Application.ProcessMessages;
      end;

      //Application.ProcessMessages;
      if dmbanco.ZConnection1.Connected then
      begin

        frmToolsOuvir := TfrmToolsOuvir.create(self);
        Application.ProcessMessages;

        frmToolsOuvir.Show;
        Application.ProcessMessages;

        frmToolsfalar.edIP.text :=  Fsetmain.IPFALAR;
        frmToolsOuvir.edIP.text :=  Fsetmain.IPFALAR;
        //Application.ProcessMessages;
        //Sleep(1000);

        frmToolsfalar.Conectar();
        frmToolsOuvir.Conectar();
        //Application.ProcessMessages;
        //dmbanco.LazSerial2.OnRxData:= nil;
        frmlog.close;
        frmLog.free;
        frmlog := nil;


        frmToolsfalar.Falar('Iniciando o servidor do Doctor, aguarde');
        Application.ProcessMessages;
        //Sleep(10000);
        //frmToolsfalar.Hide;
        //Application.ProcessMessages;
        //Finalizando o Setup
        frmToolsfalar.Falar('Equipamento pronto para operar');

        //Conexao com impressora
        frmtoolprinter := Tfrmtoolprinter.create(self);
        frmtoolprinter.show;
        frmtoolprinter.Conectar();

        //Application.ProcessMessages;
        BloqueioAcesso();
        //dmbanco.LazSerial2.OnRxData:= @LazSerial2RxData;
      end
      else
      begin
        frmToolsfalar.Falar('Atenção, houve um problema na conexão com o banco de dados, mude as configurações do servidor');
        Application.ProcessMessages;

        Application.Terminate;

      end;
      Application.ProcessMessages;

  end;
end;

procedure Tfrmmain.ChamaConfiguracao;
begin
  frmconfig :=  Tfrmconfig.create(self);
  frmToolsfalar.Falar('Configurações do Sistema');
  Application.ProcessMessages;
  if(frmLog <> nil) then
  begin
    frmLog.Free;
    frmLog := nil;
  end;
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
  if(frmLog <> nil) then
  begin
    frmLog.Free;
    frmLog := nil;
  end;
  frmOpAmostragem.showmodal();
  frmOpAmostragem.free();
  frmToolsfalar.Falar('Voltou a Tela Principal');
  Application.ProcessMessages;
end;

procedure Tfrmmain.AguardeInicializar();
begin
  frmaguarde := tfrmaguarde.create(self);

  frmaguarde.showmodal();
  frmaguarde.free;
  frmaguarde := nil;
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













end.

