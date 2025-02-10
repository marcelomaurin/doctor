unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, Menus, ExtCtrls,
  ComCtrls, banco, setmain, brobotico, funcoes, etiquetar, receitas, imp,
  config;

type

  { Tfrmmain }

  Tfrmmain = class(TForm)
    MainMenu1: TMainMenu;
    MenuItem1: TMenuItem;
    MenuItem2: TMenuItem;
    MenuItem3: TMenuItem;
    MenuItem4: TMenuItem;
    MenuItem5: TMenuItem;
    MenuItem6: TMenuItem;
    MenuItem7: TMenuItem;
    MenuItem8: TMenuItem;
    miconfig: TMenuItem;
    Separator1: TMenuItem;
    Separator2: TMenuItem;
    StatusBar1: TStatusBar;
    Timer1: TTimer;
    TrayIcon1: TTrayIcon;
    procedure FormCreate(Sender: TObject);
    procedure MenuItem2Click(Sender: TObject);
    procedure MenuItem4Click(Sender: TObject);
    procedure MenuItem5Click(Sender: TObject);
    procedure MenuItem7Click(Sender: TObject);
    procedure miconfigClick(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
  private

  public
    procedure Setup();

  end;

var
  frmmain: Tfrmmain;

implementation

{$R *.lfm}

{ Tfrmmain }

procedure Tfrmmain.MenuItem7Click(Sender: TObject);
begin
  Close;
end;

procedure Tfrmmain.miconfigClick(Sender: TObject);
begin
  frmconfig := Tfrmconfig.create(self);
  frmconfig.showmodal;
  frmconfig.free;
  frmconfig := nil;
  dmbanco.DisconectarSerial();
  dmbanco.ConectarSerial();

end;

procedure Tfrmmain.Timer1Timer(Sender: TObject);
begin
  if (dmbanco <> nil) then
  begin
       StatusBar1.Panels[0].Text := iif(dmbanco.LTCPComponent1.Active,'on','off');

  end
  else
  begin
    StatusBar1.Panels[0].Text := 'off';
  end;
end;

procedure Tfrmmain.Setup();
begin
  FSetMain := TSetMain.create();
  FSetMain.CarregaContexto();

  dmbanco := Tdmbanco.Create(self);
  dmbanco.ConectarSerial();
  dmbanco.SdpoSerial1.Open;

  Application.ProcessMessages;
  sleep(1000);

end;

procedure Tfrmmain.FormCreate(Sender: TObject);
begin
  Setup();
end;

procedure Tfrmmain.MenuItem2Click(Sender: TObject);
begin
  frmetiquetar := Tfrmetiquetar.create(self);
  frmetiquetar.showmodal();
  frmetiquetar.free;
  frmetiquetar := nil;

end;

procedure Tfrmmain.MenuItem4Click(Sender: TObject);
begin
  frmreceitas := Tfrmreceitas.create(self);
  frmreceitas.showmodal;
  frmreceitas.free;
  frmreceitas := nil;
end;

procedure Tfrmmain.MenuItem5Click(Sender: TObject);
begin
  frmbrobotico := Tfrmbrobotico.create(self);
  frmbrobotico.showmodal;
  frmbrobotico.free;
  frmbrobotico := nil;
end;

end.

