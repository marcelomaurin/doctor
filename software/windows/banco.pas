unit banco;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, ZConnection, ZDataset, SdpoSerial, lNetComponents,
  setmain, brobotico, lNet, imp;

type

  { Tdmbanco }

  Tdmbanco = class(TDataModule)
    LTCPComponent1: TLTCPComponent;
    SdpoSerial1: TSdpoSerial;
    ZConnection1: TZConnection;
    zqryExames: TZQuery;
    zqryreadaux: TZReadOnlyQuery;
    zqryCliente: TZQuery;
    zqryEtiqueta: TZQuery;
    procedure DataModuleCreate(Sender: TObject);
    procedure DataModuleDestroy(Sender: TObject);
    procedure LazSerial2RxData(Sender: TObject);
    procedure LTCPComponent1Connect(aSocket: TLSocket);
    procedure LTCPComponent1Disconnect(aSocket: TLSocket);
    procedure LTCPComponent1Receive(aSocket: TLSocket);
  private

  public
    buffer : string;
    POSESTEIRA : LongInt;
    POSFIMESTEIRA : LongInt;
    flgStart : boolean;
    Function BuscaChavePessoa( chave : string) : boolean;
    Function BuscaIDPessoa( chave : string) : integer;
    Function BuscaNomePessoa( chave : string) : String;
    Function BuscaCliente(Nome : String): boolean;
    Function BuscaTipoExame(Exame: string): boolean;
    function RegistraAmostra(idCliente : string; idExame: string) : integer;
    function PegaId() : integer;
    procedure RegistraOuve(texto: string);
    function RetornaComando(id : integer): integer;
    procedure AnalisarBuffer(const linha: string);
    procedure PHFimCurso;
    procedure CalibrarEquipamento;
    procedure TelaReceita;
    procedure RetornarBracoRobototico;
    procedure CalibrarModulo2;
    procedure CalibrarModulo1;
    procedure CadastrarPaciente;
    procedure CadastrarAmostra;
    procedure TestarAmostra;
    procedure Etiquetagem;
    procedure SairSistema;
    procedure StatusSistema;
    procedure RecebeuAssociacao(Associacao: string);
    procedure SendData( Info : string);
    procedure ConectarSerial();
    procedure DisconectarSerial();
    procedure RecebeDadosSerial(info : string);
    procedure Conectadatabase();

  end;

var
  dmbanco: Tdmbanco;



implementation

{$R *.lfm}

{ Tdmbanco }

uses main, etiquetar;

procedure Tdmbanco.AnalisarBuffer(const linha: string);
var
  posTemperatura, posHumidade, posMoveDir, posMoveEsc: longint;
  temperatura, humidade, moveDir, moveEsc: string;
  valorInt: Integer;
  valorTemperatura : double;
  valorHumidade : double;

begin
  posTemperatura := pos('TEMPERATURA:', linha);
  if posTemperatura > 0 then
  begin
    // Extrai o valor da temperatura e substitui vírgulas por pontos
    temperatura := copy(linha, posTemperatura + 12, pos(#32, linha + ' ', posTemperatura + 12) - posTemperatura - 12);
    temperatura := StringReplace(temperatura, '.', ',', [rfReplaceAll]); // Substitui vírgulas por pontos

    // Remove o caractere `#0D` se estiver presente
    if (Length(temperatura) > 0) and (temperatura[Length(temperatura)] = #13) then
      Delete(temperatura, Length(temperatura), 1);

    // Verifica se o parâmetro `temperatura` é válido
    if not TryStrToFloat(temperatura, valorTemperatura) then
    begin
      valorTemperatura := 0; // Define como 0 caso o valor seja inválido ou não informado
    end;

    if (frmbrobotico <> nil) then
    begin
      frmbrobotico.LEDTEMP.Caption := FloatToStr(valorTemperatura);
    end;
  end;


  posTemperatura := pos('Bem vindo', linha);
  if posTemperatura > 0 then
  begin
    dmbanco.flgStart := true;
  end;

  posTemperatura := pos('BRACO:1=', UpperCase(linha));
  if posTemperatura > 0 then
  begin
     // Extrai o valor da humidade e substitui vírgulas por pontos
    humidade := copy(linha, posTemperatura + 8, pos(#32, linha + ' ', posTemperatura + 8) - posTemperatura - 8);
    //humidade := StringReplace(humidade, '.', ',', [rfReplaceAll]); // Substitui vírgulas por pontos
    if (frmbrobotico <> nil) then
    begin
         frmbrobotico.ledEixo1.Caption := humidade;
    end;
  end;

  posTemperatura := pos('BRACO:2=', UpperCase(linha));
  if posTemperatura > 0 then
  begin
     // Extrai o valor da humidade e substitui vírgulas por pontos
    humidade := copy(linha, posTemperatura + 8, pos(#32, linha + ' ', posTemperatura + 8) - posTemperatura - 8);
    //humidade := StringReplace(humidade, '.', ',', [rfReplaceAll]); // Substitui vírgulas por pontos
    if (frmbrobotico <> nil) then
    begin
         frmbrobotico.ledEixo2.Caption := humidade;
    end;
  end;

  posTemperatura := pos('BRACO:3=', UpperCase(linha));
  if posTemperatura > 0 then
  begin
     // Extrai o valor da humidade e substitui vírgulas por pontos
    humidade := copy(linha, posTemperatura + 8, pos(#32, linha + ' ', posTemperatura + 8) - posTemperatura - 8);
    //humidade := StringReplace(humidade, '.', ',', [rfReplaceAll]); // Substitui vírgulas por pontos
    if (frmbrobotico <> nil) then
    begin
         frmbrobotico.ledEixo3.Caption := humidade;
    end;
  end;

  posTemperatura := pos('BRACO:4=', UpperCase(linha));
  if posTemperatura > 0 then
  begin
     // Extrai o valor da humidade e substitui vírgulas por pontos
    humidade := copy(linha, posTemperatura + 8, pos(#32, linha + ' ', posTemperatura + 8) - posTemperatura - 8);
    //humidade := StringReplace(humidade, '.', ',', [rfReplaceAll]); // Substitui vírgulas por pontos
    if (frmbrobotico <> nil) then
    begin
         frmbrobotico.ledEixo4.Caption := humidade;
    end;
  end;


  posTemperatura := pos('POSSERVA=', UpperCase(linha));
  if posTemperatura > 0 then
  begin
     // Extrai o valor da humidade e substitui vírgulas por pontos
    humidade := copy(linha, posTemperatura + 9, pos(#32, linha + ' ', posTemperatura + 9) - posTemperatura - 9);
    //humidade := StringReplace(humidade, '.', ',', [rfReplaceAll]); // Substitui vírgulas por pontos
    if (frmbrobotico <> nil) then
    begin
         frmbrobotico.LEDPOSERVA.Caption := humidade;
    end;
  end;
  posTemperatura := pos('POSERVA=', UpperCase(linha));
  if posTemperatura > 0 then
  begin
     // Extrai o valor da humidade e substitui vírgulas por pontos
    humidade := copy(linha, posTemperatura + 8, pos(#32, linha + ' ', posTemperatura + 8) - posTemperatura - 8);
    //humidade := StringReplace(humidade, '.', ',', [rfReplaceAll]); // Substitui vírgulas por pontos
    if (frmbrobotico <> nil) then
    begin
         frmbrobotico.LEDPOSERVA.Caption := humidade;
    end;
  end;

  posHumidade := pos('Humidade:', linha);
  if posHumidade > 0 then
  begin
    // Extrai o valor da humidade e substitui vírgulas por pontos
    humidade := copy(linha, posHumidade + 9, pos(#32, linha + ' ', posHumidade + 9) - posHumidade - 9);
    humidade := StringReplace(humidade, '.', ',', [rfReplaceAll]); // Substitui vírgulas por pontos

    // Remove o caractere `#0D` se estiver presente
    if (Length(humidade) > 0) and (humidade[Length(humidade)] = #13) then
      Delete(humidade, Length(humidade), 1);

    // Verifica se o parâmetro `humidade` é válido
    if not TryStrToFloat(humidade, valorHumidade) then
    begin
      valorHumidade := 0; // Define como 0 caso o valor seja inválido ou não informado
    end;

    if (frmbrobotico <> nil) then
    begin
      frmbrobotico.LEDHUM.Caption := FloatToStr(valorHumidade);
    end;
  end;



  posMoveDir := pos('POSSERVA=', linha);
  if posMoveDir > 0 then
  begin
    moveDir := copy(linha, posMoveDir + 9, pos(#32, linha + ' ', posMoveDir + 9) - posMoveDir - 9);

    // Verifica se moveDir é um valor válido e trata valores negativos
    if TryStrToInt(moveDir, valorInt) then
    begin
      if valorInt < 0 then
        valorInt := 0; // Substitui valores negativos por zero
    end
    else
      valorInt := 0; // Substitui valores inválidos por zero

    if (frmbrobotico <> nil) then
    begin
      dmbanco.POSESTEIRA := dmbanco.POSESTEIRA + valorInt;

      if (frmbrobotico <> nil) then
      begin
        frmbrobotico.Image2.Left := 32 + trunc(frmbrobotico.tbMov.Position * 0.228);
        frmbrobotico.Image2.Refresh;
        frmbrobotico.lbPosicao.Caption := IntToStr(valorInt);
      end;
    end;
  end;


  // POSFIMSERVA
  posMoveDir := pos('POSFIMSERVA=', linha);
  if posMoveDir > 0 then
  begin
    moveDir := copy(linha, posMoveDir + 12, pos(#32, linha + ' ', posMoveDir + 12) - posMoveDir - 12);

    // Verifica se moveDir é um valor válido e trata valores negativos
    if TryStrToInt(moveDir, valorInt) then
    begin
      if valorInt < 0 then
        valorInt := 0; // Substitui valores negativos por zero
    end
    else
      valorInt := 0; // Substitui valores inválidos por zero

    if (frmbrobotico <> nil) then
    begin
      dmbanco.POSFIMESTEIRA := valorInt;
      frmbrobotico.lbPOSFIMESTEIRA.Caption := IntToStr(valorInt);
      frmbrobotico.tbMov.Max := valorInt;
    end;
  end;
end;

procedure Tdmbanco.StatusSistema;
begin
  //frmToolsfalar.Falar('Tudo ok com o sistema '); //Status do equipamento
end;

procedure Tdmbanco.SairSistema;
begin
  //frmToolsfalar.Falar('Estou finalizando a aplicação, obrigado'); //Status do equipamento
  //application.Terminate;
end;

procedure Tdmbanco.Etiquetagem;
begin
  frmEtiquetar := TfrmEtiquetar.create(self);
  frmEtiquetar.showmodal();
  frmEtiquetar.free();
end;

procedure Tdmbanco.TestarAmostra;
begin
  //frmToolsfalar.Falar('Iniciando Teste de Amostra '); //Ola
  //frmToolsfalar.Falar('Informe o codigo de barras da amostra que deseja testar! '); //Ola
end;

procedure Tdmbanco.CadastrarAmostra;
begin
  //frmToolsfalar.Falar('Chamando tela de Cadastro de Amostra '); //Ola
end;

procedure Tdmbanco.CadastrarPaciente;
begin
 // frmToolsfalar.Falar('Chamando tela de Cadastro de Paciente '); //Ola
end;

procedure Tdmbanco.CalibrarModulo1;
begin
  //frmToolsfalar.Falar('Iniciando módulo de calibragem 1 '); //Ola
  (*
  if(frmlog=nil) then
  begin
       frmlog := Tfrmlog.create(self);
  end;
  frmlog.show();
  *)
  SendData('SENDMSG=1,CALIBRAR'+#10);

end;

procedure Tdmbanco.CalibrarModulo2;
begin
  //frmToolsfalar.Falar('Iniciando módulo de calibragem 2 '); //Ola

  SendData('SENDMSG=2,CALIBRAR'+#10);
end;

procedure Tdmbanco.RetornarBracoRobototico;
begin
  //frmToolsfalar.Falar('Iniciando braço robótico '); //Ola

  SendData('RETORNOCARRO'+#10);
end;

procedure Tdmbanco.TelaReceita;
begin
 // frmToolsfalar.Falar('Chamando tela de receita '); //Ola
end;

procedure Tdmbanco.CalibrarEquipamento;
begin
  //frmToolsfalar.Falar('Iniciando Calibração do equipamento '); //Ola

  SendData('CALIBRACAO'+#10);
end;


procedure Tdmbanco.PHFimCurso;
begin
 // frmToolsfalar.Falar('Posicionando PH na posicao de leitura '); //Ola

  SendData('SENDMSG=1,MOVERFIMCURSOESQ'+#10);
end;

procedure Tdmbanco.DataModuleCreate(Sender: TObject);
begin
  flgStart := false;
  buffer := '';
  POSESTEIRA := 0;
  ZConnection1.Disconnect;
  try
    {$ifdef Darwin}
        ZConnection1.LibraryLocation := '/usr/lib/aarch64-linux-gnu/libmariadb.so';
    {$ENDIF}
    {$IFDEF LINUX}
      {$IFDEF CPUARM}
        ZConnection1.LibraryLocation := '/usr/lib/aarch64-linux-gnu/libmariadb.so';
      {$ENDIF}
      {$IFDEF X86_64}
        ZConnection1.LibraryLocation := '/usr/lib/x86_64-linux-gnu/libmariadb.so';
      {$ELSE}
        ZConnection1.LibraryLocation := '/usr/lib/aarch64-linux-gnu/libmariadb.so';
      {$ENDIF}

    {$ENDIF}
    {$IFDEF WINDOWS}
        ZConnection1.LibraryLocation := 'D:\projetos\maurinsoft\doctor\software\raspberry\libs\mysql\win64\libmysql64.dll';
    {$ENDIF}
    fImp := TImp.create(SdpoSerial1);
    Conectadatabase();
  finally
  end;
end;

procedure Tdmbanco.DataModuleDestroy(Sender: TObject);
begin
  //dmbanco.LazSerial2.close;
  LTCPComponent1.Disconnect(false);

end;

procedure Tdmbanco.LazSerial2RxData(Sender: TObject);
begin

end;

procedure Tdmbanco.LTCPComponent1Connect(aSocket: TLSocket);
begin
  //showmessage('Conectou');
end;

procedure Tdmbanco.LTCPComponent1Disconnect(aSocket: TLSocket);
begin
  //showmessage('Desconectou');
end;

procedure Tdmbanco.LTCPComponent1Receive(aSocket: TLSocket);
var
  info : string;
begin
  aSocket.GetMessage(info);
  RecebeDadosSerial(info);
end;

function Tdmbanco.BuscaChavePessoa(chave: string): boolean;
begin

    zqryreadaux.close;
    zqryreadaux.sql.text := 'select * from pessoas where chave = "'+StringReplace(chave, #13, '', [rfReplaceAll])+'" ';
    zqryreadaux.Prepare;
    zqryreadaux.Active:= true;
    if(zqryreadaux.Active) then
    begin
        Result := zqryreadaux.RecordCount<>0;

    end
    else
    begin
      //showmessage('Nao conseguiu acesso ao banco');
      //frmToolsfalar.Falar('Aplicação não conseguiu acessar o banco de dados');
      //Application.ProcessMessages;
    end;
end;

function Tdmbanco.BuscaIDPessoa(chave: string): integer;
begin
  zqryreadaux.close;
  zqryreadaux.sql.text := 'select * from pessoas where chave = "'+StringReplace(chave, #13, '', [rfReplaceAll])+'" ';
  zqryreadaux.Prepare;
  zqryreadaux.Active:= true;
  if(zqryreadaux.Active) then
  begin
      Result := zqryreadaux.FieldByName('id').asinteger;

  end
  else
  begin
    //showmessage('Nao conseguiu acesso ao banco');
    //frmToolsfalar.Falar('Aplicação não conseguiu acessar o banco de dados');
    Result := 0;
    //Application.ProcessMessages;
  end;
end;

function Tdmbanco.BuscaNomePessoa(chave: string): String;
begin
  zqryreadaux.close;
  zqryreadaux.sql.text := 'select * from pessoas where chave = "'+StringReplace(chave, #13, '', [rfReplaceAll])+'" ';
  zqryreadaux.Prepare;
  zqryreadaux.Active:= true;
  if(zqryreadaux.Active) then
  begin
      Result := zqryreadaux.FieldByName('nome').asstring;

  end
  else
  begin
    //showmessage('Nao conseguiu acesso ao banco');
    //frmToolsfalar.Falar('Aplicação não conseguiu acessar o banco de dados');
    result := '';
    //Application.ProcessMessages;
  end;
end;

function Tdmbanco.BuscaCliente(Nome: String): boolean;
begin
  zqryCliente.close;
  zqryCliente.sql.text := 'select * from clientes where nome like "%'+Nome+'%"';
  zqryCliente.Prepare;
  zqryCliente.Open;

end;

function Tdmbanco.BuscaTipoExame(Exame: string): boolean;
begin
  zqryExames.close;
  zqryExames.sql.text := 'select * from exames where nome like "%'+Exame+'%"';
  zqryExames.Prepare;
  zqryExames.Open;
end;

function Tdmbanco.RegistraAmostra(idCliente: string; idExame: string): integer;
var
  id: integer;
  ZQueryGetID: TZQuery;
begin
  ZConnection1.StartTransaction;
  try
    // Insere a amostra
    zqryExames.Close;
    zqryExames.SQL.Text := 'INSERT INTO amostras (id_cliente, codbar, status) VALUES (:idCliente, "0", 0)';
    zqryExames.Params.ParamByName('idCliente').AsString := idCliente;
    zqryExames.ExecSQL;

    // Recupera o ID gerado pelo último INSERT
    ZQueryGetID := TZQuery.Create(nil);
    try
      ZQueryGetID.Connection := ZConnection1;
      ZQueryGetID.SQL.Text := 'SELECT LAST_INSERT_ID() AS id';
      ZQueryGetID.Open;

      // Verifica se o ID foi recuperado corretamente
      id := ZQueryGetID.FieldByName('id').AsInteger;
      if id = 0 then
        raise Exception.Create('Erro ao recuperar o ID gerado: ID retornado é zero.');
    finally
      ZQueryGetID.Free;
    end;

    // Atualiza o código de barras
    zqryExames.Close;
    zqryExames.SQL.Text := 'UPDATE amostras SET codbar = :codbar WHERE id = :id';
    zqryExames.Params.ParamByName('codbar').AsString := IntToStr(id);
    zqryExames.Params.ParamByName('id').AsInteger := id;
    zqryExames.ExecSQL;

    // Commit da transação
    ZConnection1.Commit;

    // Retorna o ID gerado
    result := id;
  except
    on E: Exception do
    begin
      ZConnection1.Rollback;
      //ShowMessage('Erro: ' + E.Message);
      raise; // Relevanta a exceção para o chamador lidar com ela
    end;
  end;
end;


function Tdmbanco.PegaId: integer;
var
  SelectIDSQL: string;
begin
   // Recuperar o ID gerado
   SelectIDSQL := 'SELECT LAST_INSERT_ID() AS id';
   //zqryreadaux.Connection := ZConnection;
   zqryreadaux.SQL.Text := SelectIDSQL;
   zqryreadaux.Open;
   // Obter o ID gerado
   result := zqryreadaux.FieldByName('id').AsInteger;
end;

procedure Tdmbanco.RegistraOuve(texto: string);
begin
  zqryCliente.close;
  zqryCliente.sql.text := 'insert into logouve (texto,dtcad) values  ("'+texto+'",now())';
  zqryCliente.Prepare;
  zqryCliente.ExecSQL;
end;

function Tdmbanco.RetornaComando(id: integer): integer;
var
  SelectIDSQL: string;
begin
   zqryreadaux.close;
   // Recuperar o ID gerado
   SelectIDSQL := 'SELECT idcomando from logouve where id = '+inttostr(id);
   //zqryreadaux.Connection := ZConnection;
   zqryreadaux.SQL.Text := SelectIDSQL;
   zqryreadaux.Open;
   // Obter o ID gerado
   result := zqryreadaux.FieldByName('idcomando').AsInteger;
   zqryreadaux.close;
end;

procedure Tdmbanco.RecebeuAssociacao(Associacao: string);
var
  comando: integer;
begin
  //Recebeu comando
  comando := dmbanco.RetornaComando(strtoint(Associacao));

  case comando of
    1: begin

    end;
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
   14: CalibrarEquipamento();
   15: PHFimCurso();
  else
        //frmToolsfalar.Falar('Comando não encontrado '); //Comando nao encontrado
    begin

    end;
  end;

end;

procedure Tdmbanco.SendData(Info: string);
begin
  //LazSerial2.WriteData(info);
  LTCPComponent1.SendMessage(info);

end;

procedure Tdmbanco.ConectarSerial;
begin
    //LazSerial2.Device:= FSetMain.SerialPort;
    //LazSerial2.open;
  //FSetMain.SerialPort := '192.168.100.12';
  //FSetMain.SalvaContexto(true);
  //LTCPComponent1.Connect(FSetMain.SerialPort,8101);

  LTCPComponent1.Disconnect(true);
  LTCPComponent1.Connect(FSetMain.Servidor,8101);
  SdpoSerial1.close;
  SdpoSerial1.Device:= fsetmain.SerialPort;
  SdpoSerial1.Open;
  fimp.tipoimp := CTipoIMP(FSetMain.TipoImp);

end;

procedure Tdmbanco.DisconectarSerial;
begin
  //LazSerial2.close;
  LTCPComponent1.Disconnect(false);
  SdpoSerial1.close;
end;

procedure Tdmbanco.RecebeDadosSerial(info: string);
var
    linha: string;
    posicaoLF: integer;
begin
    if (info <> '') then
    begin
      //info := LazSerial2.ReadData;
      buffer := buffer + info; // Adiciona os dados ao buffer

      repeat
        posicaoLF := pos(#10, buffer); // Procura por \n (Line Feed)
        if posicaoLF > 0 then
        begin
          linha := copy(buffer, 1, posicaoLF - 1); // Texto antes do \n
          buffer := copy(buffer,posicaoLF+1,Length(buffer));
          // Chama a rotina AnalisarBuffer com a linha extraída
          AnalisarBuffer(linha);

        end;
      until posicaoLF = 0;
    end;
    //Application.ProcessMessages;
end;

procedure Tdmbanco.Conectadatabase();
begin
  ZConnection1.HostName := FSetMain.HostnameMy;
  ZConnection1.Database := FSetMain.BancoMy;
  ZConnection1.Password:= FSetMain.PasswordMy;
  ZConnection1.User:= FSetMain.UsernameMy;
  ZConnection1.Connect;
end;

end.

