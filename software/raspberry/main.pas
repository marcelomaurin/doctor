unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  setmain, Toolsfalar;

type

  { Tfrmmain }

  Tfrmmain = class(TForm)
    Image1: TImage;
    Image2: TImage;
    Image3: TImage;
    Image4: TImage;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    procedure Setup();

  public

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

procedure Tfrmmain.Setup;
begin
  FSetMain := TSetMain.create();
  FSetMain.CarregaContexto();
  frmToolsfalar :=   TfrmToolsfalar.create(self);
  frmToolsfalar.Show;


  frmToolsfalar.edIP.text :=  Fsetmain.IPFALAR;

  frmToolsfalar.Conectar();
  Application.ProcessMessages;
  Sleep(1000);
  frmToolsfalar.Falar('Iniciando o servidor do Doctor, aguarde.');
  Application.ProcessMessages;
  Sleep(10000);
  //Finalizando o Setup
  frmToolsfalar.Falar('Equipamento pronto para operar.');
  Application.ProcessMessages;
end;

end.

