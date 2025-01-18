unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  setmain, Toolsfalar, config, banco, bloqueio, OpAmostragem, imp, LazSerial,
  ToolsOuvir, Etiquetar, splash, brobotico, log, aguarde,
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
    LazSerial2: TSdpoSerial;
    LazSerial1: TSdpoSerial;
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
    buffer : string;
    POSESTEIRA : LongInt;
    POSFIMESTEIRA : LongInt;
    flgStart : boolean;

    procedure AguardeInicializar();
    procedure BloqueioAcesso();
    procedure RecebeuPergunta(Pergunta: string);
    procedure RecebeuAssociacao(Associacao : string);
    procedure StatusSistema();
    procedure SairSistema();
    procedure Etiquetagem();
    procedure TestarAmostra();
    procedure CadastrarAmostra();
    procedure CadastrarPaciente();
    procedure CalibrarModulo1();
    procedure CalibrarModulo2();
    procedure RetornarBracoRobototico();
    procedure TelaReceita();
    procedure AnalisarBuffer(const linha: string);

  end;

var
  frmmain: Tfrmmain;

implementation

{$R *.lfm}

{ Tfrmmain }

procedure Tfrmmain.FormCreate(Sender: TObject);
begin
  flgStart := false;
  frmsplash := tfrmsplash.create(self);
  frmsplash.showmodal();
  Setup();
  frmsplash.free;
  buffer := '';
  POSESTEIRA := 0;

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
var
    info, linha: string;
    posicaoLF: integer;
begin
    if LazSerial2.DataAvailable then
    begin
      info := LazSerial2.ReadData;
      buffer := buffer + info; // Adiciona os dados ao buffer

      repeat
        posicaoLF := pos(#10, buffer); // Procura por \n (Line Feed)
        if posicaoLF > 0 then
        begin
          linha := copy(buffer, 1, posicaoLF - 1); // Texto antes do \n
          buffer := copy(buffer,posicaoLF+1,Length(buffer));
          // Chama a rotina AnalisarBuffer com a linha extraída
          AnalisarBuffer(linha);
          if (frmlog <> nil) then
          begin
            frmlog.melog.Append(linha);
            frmlog.melog.SelStart := Length(frmlog.melog.Text); // Move o cursor para o final
            frmlog.melog.SelLength := 0; // Remove qualquer seleção de texto visível
            //frmlog.melog.SetFocus; // Opcional: garante o foco no TMemo
          end;
        end;
      until posicaoLF = 0;
    end;
    Application.ProcessMessages;
end;




procedure Tfrmmain.SdpoSerial1RxData(Sender: TObject);
begin

end;

procedure Tfrmmain.Setup;
begin
  FSetMain := TSetMain.create();
  FSetMain.CarregaContexto();
  dmbanco := Tdmbanco.Create(self);
  frmToolsfalar :=   TfrmToolsfalar.create(self);
  frmToolsfalar.Show;
  Application.ProcessMessages;
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
  Application.ProcessMessages;
  frmlog := Tfrmlog.create(self); //Cria o log de eventos
  frmlog.show;
  Application.ProcessMessages;
  try
    LazSerial2.Device:= FSetMain.SerialPort;
    LazSerial2.open;
    AguardeInicializar();
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
    LazSerial2.OnRxData:= nil;
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
    //Application.ProcessMessages;
    BloqueioAcesso();
    LazSerial2.OnRxData:= @LazSerial2RxData;
  end
  else
  begin
    frmToolsfalar.Falar('Atenção, houve um problema na conexão com o banco de dados, mude as configurações do servidor');
    Application.ProcessMessages;

    Application.Terminate;

  end;
  Application.ProcessMessages;


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
    6: TestarAmostra();
    8: CadastrarAmostra();
    9: CadastrarPaciente();
   10: CalibrarModulo1();
   11: CalibrarModulo2();
   12: RetornarBracoRobototico();
   13: TelaReceita();
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

procedure Tfrmmain.TestarAmostra;
begin
  frmToolsfalar.Falar('Iniciando Teste de Amostra '); //Ola
  frmToolsfalar.Falar('Informe o codigo de barras da amostra que deseja testar! '); //Ola
end;

procedure Tfrmmain.CadastrarAmostra;
begin
  frmToolsfalar.Falar('Chamando tela de Cadastro de Amostra '); //Ola
end;

procedure Tfrmmain.CadastrarPaciente;
begin
  frmToolsfalar.Falar('Chamando tela de Cadastro de Paciente '); //Ola
end;

procedure Tfrmmain.CalibrarModulo1;
begin
  frmToolsfalar.Falar('Iniciando módulo de calibragem 1 '); //Ola
end;

procedure Tfrmmain.CalibrarModulo2;
begin
  frmToolsfalar.Falar('Iniciando módulo de calibragem 2 '); //Ola
end;

procedure Tfrmmain.RetornarBracoRobototico;
begin
  frmToolsfalar.Falar('Iniciando braço robótico '); //Ola
end;

procedure Tfrmmain.TelaReceita;
begin
  frmToolsfalar.Falar('Chamando tela de receita '); //Ola
end;


procedure Tfrmmain.AnalisarBuffer(const linha: string);
var
  posTemperatura, posHumidade, posMoveDir, posMoveEsc: longint;
  temperatura, humidade, moveDir, moveEsc: string;
begin
  posTemperatura := pos('TEMPERATURA:', linha);
  if posTemperatura > 0 then
  begin
    temperatura := copy(linha, posTemperatura + 12, pos(#32, linha + ' ', posTemperatura + 12) - posTemperatura - 12);
    if (frmbrobotico <> nil) then
    begin
      frmbrobotico.LEDTEMP.Caption := temperatura;
    end;
  end;

  posTemperatura := pos('Bem vindo', linha);
  if posTemperatura > 0 then
  begin
    flgStart := true;
  end;

  posHumidade := pos('Humidade:', linha);
  if posHumidade > 0 then
  begin
    humidade := copy(linha, posHumidade + 9, pos(#32, linha + ' ', posHumidade + 9) - posHumidade - 9);
    if (frmbrobotico <> nil) then
    begin
      frmbrobotico.LEDHUM.Caption := humidade;
    end;
  end;

  posMoveDir := pos('POSSERVA=', linha);
  if posMoveDir > 0 then
  begin
    moveDir := copy(linha, posMoveDir + 9, pos(#32, linha + ' ', posMoveDir + 9) - posMoveDir - 9);
    if (frmbrobotico <> nil) then
    begin
      POSESTEIRA := POSESTEIRA + strtoint(moveDir);
      if  (frmbrobotico<> nil) then
      begin
           frmbrobotico.Image2.Left:= 32+trunc(frmbrobotico.tbMov.Position * 0.228);
           frmbrobotico.Image2.refresh;
           //frmbrobotico.Image2.Left := 24 + trunc(tbMov.Position*0.28);
           frmbrobotico.lbPosicao.Caption:=  moveDir;
           //frmbrobotico.lbPosicao. := POSESTEIRA;
      end;
    end;
  end;
  //POSFIMSERVA
  posMoveDir := pos('POSFIMSERVA=', linha);
  if posMoveDir > 0 then
  begin
    moveDir := copy(linha, posMoveDir + 12, pos(#32, linha + ' ', posMoveDir + 12) - posMoveDir - 12);
    if (frmbrobotico <> nil) then
    begin
       POSFIMESTEIRA := StrToInt64(moveDir);
       frmbrobotico.lbPOSFIMESTEIRA.Caption := moveDir;
       frmbrobotico.tbMov.Max:= POSFIMESTEIRA;
    end;
  end;
end;



end.

