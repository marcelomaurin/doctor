unit receitas;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ComCtrls, StdCtrls,
  banco;


type

  { Tfrmreceitas }

  Tfrmreceitas = class(TForm)
    Button1: TButton;
    cbmodulo: TComboBox;
    cbcomando: TComboBox;
    Label1: TLabel;
    Label2: TLabel;
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private

  public

  end;

var
  frmreceitas: Tfrmreceitas;

implementation

{$R *.lfm}

{ Tfrmreceitas }

procedure Tfrmreceitas.FormCreate(Sender: TObject);
begin
  PageControl1.ActivePage := TabSheet1;

end;

procedure Tfrmreceitas.FormDestroy(Sender: TObject);
begin
    if (frmlog <> nil) then
    begin
        frmlog.close;
        frmlog.free;
        frmlog := nil;
    end;
end;

procedure Tfrmreceitas.Button1Click(Sender: TObject);
begin
  if(dmbanco.LTCPComponent1.Connected) then
  begin
       //dmbanco.LazSerial2.OnRxData:= @frmmain.LazSerial2RxData;
       dmbanco.SendData('SENDMSG='+cbmodulo.Items[cbmodulo.ItemIndex]+','+cbcomando.Items[cbcomando.ItemIndex]+#10);
  end;
end;

end.

