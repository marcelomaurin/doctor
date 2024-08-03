unit banco;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, ZConnection, ZDataset, Toolsfalar;

type

  { Tdmbanco }

  Tdmbanco = class(TDataModule)
    ZConnection1: TZConnection;
    zqryExames: TZQuery;
    zqryreadaux: TZReadOnlyQuery;
    zqryCliente: TZQuery;
    zqryEtiqueta: TZQuery;
    procedure DataModuleCreate(Sender: TObject);
  private

  public
    Function BuscaChavePessoa( chave : string) : boolean;
    Function BuscaIDPessoa( chave : string) : integer;
    Function BuscaNomePessoa( chave : string) : String;
    Function BuscaCliente(Nome : String): boolean;
    Function BuscaTipoExame(Exame: string): boolean;
    function RegistraAmostra(idCliente : string; idExame: string) : integer;
    function PegaId() : integer;
    procedure RegistraOuve(texto: string);
    function RetornaComando(id : integer): integer;

  end;

var
  dmbanco: Tdmbanco;

implementation

{$R *.lfm}

{ Tdmbanco }

procedure Tdmbanco.DataModuleCreate(Sender: TObject);
begin
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
    ZConnection1.Connect;


  finally
  end;
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
      frmToolsfalar.Falar('Aplicação não conseguiu acessar o banco de dados');
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
    frmToolsfalar.Falar('Aplicação não conseguiu acessar o banco de dados');
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
    frmToolsfalar.Falar('Aplicação não conseguiu acessar o banco de dados');
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

end.

