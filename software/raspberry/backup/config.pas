unit config;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  toolsfalar, cfgspeak, cfgImp, brobotico;

type

  { Tfrmconfig }

  Tfrmconfig = class(TForm)
    Image1: TImage;
    Image2: TImage;
    Image3: TImage;
    Image4: TImage;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure Image1Click(Sender: TObject);
    procedure Image1MouseEnter(Sender: TObject);
    procedure Image2Click(Sender: TObject);
    procedure Image2MouseEnter(Sender: TObject);
    procedure Image3Click(Sender: TObject);
    procedure Image4Click(Sender: TObject);
  private

  public

  end;

var
  frmconfig: Tfrmconfig;

implementation

{$R *.lfm}

{ Tfrmconfig }

procedure Tfrmconfig.Image1MouseEnter(Sender: TObject);
begin
  frmToolsfalar.Falar('Configurações de Servidores');
  Application.ProcessMessages;
end;

procedure Tfrmconfig.Image2Click(Sender: TObject);
begin

end;

procedure Tfrmconfig.Image1Click(Sender: TObject);
begin
  frmcfgspeak := Tfrmcfgspeak.create(self);
  frmcfgspeak.showmodal;

  frmcfgspeak.free;
end;

procedure Tfrmconfig.FormCreate(Sender: TObject);
begin

end;

procedure Tfrmconfig.Image2MouseEnter(Sender: TObject);
begin
   frmToolsfalar.Falar('Configurações de Banco de dados');
  Application.ProcessMessages;
end;

procedure Tfrmconfig.Image3Click(Sender: TObject);
begin
  frmcfgImp := TfrmcfgImp.create(self);
  frmcfgImp.showmodal;

  frmcfgImp.free;
end;

procedure Tfrmconfig.Image4Click(Sender: TObject);
begin
  frmbrobotico := Tfrmbrobotico.create(self);
  frmbrobotico.showmodal;

  frmcfgImp.free;
end;

end.

