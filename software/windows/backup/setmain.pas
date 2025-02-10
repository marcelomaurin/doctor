//Objetivo construir os parametros de setup da classe principal
//Criado por Marcelo Maurin Martins
//Data:07/02/2021

unit setmain;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, funcoes, graphics;

const
  filename = 'operador1.cfg';

type
  { TSetMain }

  TSetMain = class(TObject)
  private
    arquivo: TStringList;
    ckdevice: boolean;
    FPosX: integer;
    FPosY: integer;
    FFixar: boolean;
    FStay: boolean;
    FLastFiles: string;
    FPATH: string;
    FHeight: integer;
    FWidth: integer;
    FFONT: TFont;
    FCHATGPT: string;
    FDllPath: string;
    FDllMyPath: string;
    FDllPostPath: string;
    FComPrinter: string;
    FDuploclick: integer;
    FSerialPort: string;   // Porta do Arduino
    FServidor: string;     // Novo campo: Servidor com valor padrão '192.168.100.12'

    FRunScript: string;    // Script de Compilação
    FDebugScript: string;  // Script de Debug
    FCleanScript: string;  // Script de Limpeza
    FInstall: string;      // Script de Instalação
    FCompile: string;      // Script de Compilação

    FHostnameMy: string;
    FBancoMy: string;
    FUsernameMy: string;
    FPasswordMy: string;

    FHostnamePost: string;
    FBancoPOST: string;
    FUsernamePost: string;
    FPasswordPost: string;
    FSchemaPost: string;
    FToolsFalar: Boolean;

    FIPFalar: string;

    // Procedimentos de set (alguns campos utilizam setter)
    procedure SetDevice(const Value: Boolean);
    procedure SetPOSX(value: integer);
    procedure SetPOSY(value: integer);
    procedure SetFixar(value: boolean);
    procedure SetStay(value: boolean);
    procedure SetLastFiles(value: string);
    procedure SetFont(value: TFont);
    procedure SetCHATGPT(value: string);
    procedure SetDllPath(value: string);
    procedure SetDllMyPath(value: string);
    procedure SetDllPostPath(value: string);
    procedure SetToolsFalar(value: boolean);
    procedure Default();
  public
    constructor create();
    destructor Destroy(); override;
    procedure SalvaContexto(flag: boolean);
    procedure CarregaContexto();
    procedure IdentificaArquivo(flag: boolean);
    property device: boolean read ckdevice write SetDevice;
    property posx: integer read FPosX write SetPOSX;
    property posy: integer read FPosY write SetPOSY;
    property fixar: boolean read FFixar write SetFixar;
    property stay: boolean read FStay write SetStay;
    property lastfiles: string read FLastFiles write SetLastFiles;
    property Height: integer read FHeight write FHeight;
    property Width: integer read FWidth write FWidth;
    property RunScript: string read FRunScript write FRunScript;
    property DebugScript: string read FDebugScript write FDebugScript;
    property CleanScript: string read FCleanScript write FCleanScript;
    property Install: string read FInstall write FInstall;
    property Compile: string read FCompile write FCompile;
    property Font: TFont read FFont write SetFont;
    property CHATGPT: string read FCHATGPT write SetCHATGPT;
    property DLLPath: string read FDllPath write SetDllPath;
    property DLLMyPath: string read FDllMyPath write SetDllMyPath;
    property DLLPostPath: string read FDllPostPath write SetDllPostPath;

    property HostnameMy: string read FHostnameMy write FHostnameMy;
    property BancoMy: string read FBancoMy write FBancoMy;
    property UsernameMy: string read FUsernameMy write FUsernameMy;
    property PasswordMy: string read FPasswordMy write FPasswordMy;
    property HostnamePost: string read FHostnamePost write FHostnamePost;
    property BancoPOST: string read FBancoPOST write FBancoPOST;
    property UsernamePost: string read FUsernamePost write FUsernamePost;
    property PasswordPost: string read FPasswordPost write FPasswordPost;
    property SchemaPost: string read FSchemaPost write FSchemaPost;
    property ToolsFalar: Boolean read FToolsFalar write SetToolsFalar;
    property IPFALAR: string read FIPFalar write FIPFALAR;
    property ComPrinter: string read FComPrinter write FComPrinter;
    property Duploclick: integer read FDuploclick write FDuploclick;
    property SerialPort: string read FSerialPort write FSerialPort;
    property Servidor: string read FServidor write FServidor;  // Propriedade do novo campo
  end;

var
  FSetMain: TSetMain;

implementation

procedure TSetMain.SetDevice(const Value: Boolean);
begin
  ckdevice := Value;
end;

// Valores default do código
procedure TSetMain.Default();
begin
  ckdevice := false;
  fixar := false;
  stay := false;
  FPosX := 100;
  FPosY := 100;
  FFixar := false;
  FStay := false;
  FIPFALAR := '127.0.0.1';
  FDuploclick := 1; // Ativo como padrão

  FDllPath := ExtractFilePath(ApplicationName);
  FDllMyPath := ExtractFilePath(ApplicationName);
  FDllPostPath := ExtractFilePath(ApplicationName);
  // FLastFiles := '';
  // FPATH : string;
  FHeight := 400;
  FWidth := 400;
  FRunScript := '';   // Script de Run
  FDebugScript := ''; // Script de Debug
  FCleanScript := ''; // Script de Limpeza
  FInstall := '';     // Script de Instalação
  FCompile := '';     // Script de Compilação
  if FFont = nil then
  begin
    FFONT := TFont.create();
  end;
  FCHATGPT := ''; // CHATGPT TOKEN
  FToolsFalar := false;
  {$ifdef Darwin}
    FComPrinter := '/dev/ttyS0';
    FSerialPort := '/dev/ttyUSB0';
  {$ENDIF}
  {$IFDEF LINUX}
    FComPrinter := '/dev/ttyS0';
    FSerialPort := '/dev/ttyUSB0';
  {$ENDIF}
  {$IFDEF WINDOWS}
    FComPrinter := 'COM4';
    FSerialPort := '192.168.100.12';
  {$ENDIF}
  // Definindo o valor padrão para o novo campo Servidor
  FServidor := '192.168.100.12';
end;

procedure TSetMain.SetPOSX(value: integer);
begin
  FPosX := value;
end;

procedure TSetMain.SetPOSY(value: integer);
begin
  FPosY := value;
end;

procedure TSetMain.SetFixar(value: boolean);
begin
  FFixar := value;
end;

procedure TSetMain.SetStay(value: boolean);
begin
  FStay := value;
end;

procedure TSetMain.SetLastFiles(value: string);
begin
  FLastFiles := value;
end;

procedure TSetMain.SetFont(value: TFont);
begin
  // StringToFont(value, FFONT);
  FFont := value;
end;

procedure TSetMain.SetCHATGPT(value: string);
begin
  FCHATGPT := value;
end;

procedure TSetMain.SetDllPath(value: string);
begin
  FDllPath := value;
end;

procedure TSetMain.SetDllMyPath(value: string);
begin
  FDllMyPath := value;
end;

procedure TSetMain.SetDllPostPath(value: string);
begin
  FDllPostPath := value;
end;

procedure TSetMain.SetToolsFalar(value: boolean);
begin
  FToolsFalar := value;
end;

procedure TSetMain.CarregaContexto();
var
  posicao: integer;
begin
  if BuscaChave(arquivo, 'DEVICE:', posicao) then
    ckdevice := (RetiraInfo(arquivo.Strings[posicao]) = '1');
  if BuscaChave(arquivo, 'POSX:', posicao) then
    FPosX := StrToInt(RetiraInfo(arquivo.Strings[posicao]));
  if BuscaChave(arquivo, 'POSY:', posicao) then
    FPosY := StrToInt(RetiraInfo(arquivo.Strings[posicao]));
  if BuscaChave(arquivo, 'FIXAR:', posicao) then
    FFixar := StrToBool(RetiraInfo(arquivo.Strings[posicao]));
  if BuscaChave(arquivo, 'STAY:', posicao) then
    FStay := StrToBool(RetiraInfo(arquivo.Strings[posicao]));
  if BuscaChave(arquivo, 'LASTFILES:', posicao) then
    FLastFiles := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'HEIGHT:', posicao) then
    FHeight := StrToInt(RetiraInfo(arquivo.Strings[posicao]));
  if BuscaChave(arquivo, 'WIDTH:', posicao) then
    FWidth := StrToInt(RetiraInfo(arquivo.Strings[posicao]));
  if BuscaChave(arquivo, 'RUNSCRIPT:', posicao) then
    FRunScript := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'DEBUGSCRIPT:', posicao) then
    FDebugScript := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'CLEANSCRIPT:', posicao) then
    FCleanScript := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'INSTALLSCRIPT:', posicao) then
    FInstall := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'COMPILESCRIPT:', posicao) then
    FCompile := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'FONT:', posicao) then
    StringToFont(RetiraInfo(arquivo.Strings[posicao]), FFONT);
  if BuscaChave(arquivo, 'CHATGPT:', posicao) then
    FCHATGPT := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'DLLPATH:', posicao) then
    FDllPath := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'DLLMYPATH:', posicao) then
    FDllMyPath := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'DLLPOSTPATH:', posicao) then
    FDllPostPath := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'HOSTNAMEMY:', posicao) then
    FHostnameMy := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'BANCOMY:', posicao) then
    FBancoMy := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'USERNAMEMY:', posicao) then
    FUsernameMy := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'PASSWORDMY:', posicao) then
    FPasswordMy := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'HOSTNAMEPOST:', posicao) then
    FHostnamePost := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'BANCOPOST:', posicao) then
    FBancoPOST := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'USERNAMEPOST:', posicao) then
    FUsernamePost := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'PASSWORDPOST:', posicao) then
    FPasswordPost := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'SCHEMAPOST:', posicao) then
    FSchemaPost := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'TOOLSFALAR:', posicao) then
    FToolsFalar := iif(RetiraInfo(arquivo.Strings[posicao]) = '0', false, true);
  if BuscaChave(arquivo, 'IPFALAR:', posicao) then
    FIPFALAR := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'COMPRINTER:', posicao) then
    FComPrinter := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'SERIALPORT:', posicao) then
    FSerialPort := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'SERVIDOR:', posicao) then
    FServidor := RetiraInfo(arquivo.Strings[posicao]);
  if BuscaChave(arquivo, 'DUPLOCLICK:', posicao) then
    FDuploclick := StrToInt(RetiraInfo(arquivo.Strings[posicao]));
end;

procedure TSetMain.IdentificaArquivo(flag: boolean);
begin
  // filename := 'Work' + FormatDateTime('ddmmyy', now()) + '.cfg';
  {$ifdef Darwin}
    // Não testado ainda
    FPATH := GetAppConfigDir(false);
    if not(FileExists(FPATH)) then
      CreateDir(FPATH);
  {$ENDIF}
  {$IFDEF LINUX}
    // FPATH := GetUserDir();
    FPATH := GetAppConfigDir(false);
    if not(FileExists(FPATH)) then
      CreateDir(FPATH);
  {$ENDIF}
  {$IFDEF WINDOWS}
    FPATH := GetAppConfigDir(false);
    if not(FileExists(FPATH)) then
      CreateDir(FPATH);
  {$ENDIF}
  if FileExists(FPATH + filename) then
  begin
    arquivo.LoadFromFile(FPATH + filename);
    CarregaContexto();
  end
  else
  begin
    Default();
    // SalvaContexto(false);
  end;
end;

// Método construtor
constructor TSetMain.create();
begin
  arquivo := TStringList.Create();
  FFONT := TFont.Create();
  IdentificaArquivo(true);
end;

procedure TSetMain.SalvaContexto(flag: boolean);
begin
  if flag then
    IdentificaArquivo(false);
  // filename := 'Work' + FormatDateTime('ddmmyy', now()) + '.cfg';
  arquivo.Clear;
  arquivo.Append('DEVICE:' + iif(ckdevice, '1', '0'));
  arquivo.Append('POSX:' + IntToStr(FPosX));
  arquivo.Append('POSY:' + IntToStr(FPosY));
  arquivo.Append('FIXAR:' + BoolToStr(FFixar));
  arquivo.Append('STAY:' + BoolToStr(FStay));
  arquivo.Append('LASTFILES:' + FLastFiles);
  arquivo.Append('HEIGHT:' + IntToStr(FHeight));
  arquivo.Append('WIDTH:' + IntToStr(FWidth));
  arquivo.Append('RUNSCRIPT:' + FRunScript);
  arquivo.Append('DEBUGSCRIPT:' + FDebugScript);
  arquivo.Append('CLEANSCRIPT:' + FCleanScript);
  arquivo.Append('INSTALLSCRIPT:' + FInstall);
  arquivo.Append('COMPILESCRIPT:' + FCompile);
  arquivo.Append('FONT:' + FontToString(FFONT));
  arquivo.Append('CHATGPT:' + FCHATGPT);
  arquivo.Append('DLLPATH:' + FDllPath);
  arquivo.Append('DLLMYPATH:' + FDllMyPath);
  arquivo.Append('DLLPOSTPATH:' + FDllPostPath);
  arquivo.Append('HOSTNAMEMY:' + FHostnameMy);
  arquivo.Append('BANCOMY:' + FBancoMy);
  arquivo.Append('USERNAMEMY:' + FUsernameMy);
  arquivo.Append('PASSWORDMY:' + FPasswordMy);
  arquivo.Append('HOSTNAMEPOST:' + FHostnamePost);
  arquivo.Append('BANCOPOST:' + FBancoPOST);
  arquivo.Append('USERNAMEPOST:' + FUsernamePost);
  arquivo.Append('PASSWORDPOST:' + FPasswordPost);
  arquivo.Append('SCHEMAPOST:' + FSchemaPost);
  arquivo.Append('TOOLSFALAR:' + iif(FToolsFalar, '1', '0'));
  arquivo.Append('IPFALAR:' + FIPFALAR);
  arquivo.Append('COMPRINTER:' + FComPrinter);
  arquivo.Append('SERIALPORT:' + FSerialPort);
  arquivo.Append('SERVIDOR:' + FServidor);
  arquivo.Append('DUPLOCLICK:' + IntToStr(FDuploclick));
  arquivo.SaveToFile(FPATH + filename);
end;

destructor TSetMain.Destroy;
begin
  // SalvaContexto(false);
  arquivo.Free;
  arquivo := nil;
  FFONT.Free;
  inherited Destroy;
end;

end.

