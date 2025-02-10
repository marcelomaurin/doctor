unit imp;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, imp_generico, imp_elgini9, imp_qr203,LazSerial, funcoes,
  SdpoSerial;



type CTipoIMP = (TI_DRIVER, TI_SERIAL,  TI_BLUETOOTH);
type CModeloIMP = (MI_ELGINI9, MI_GENERIC, MI_IMP59COL);

type CFormat = (FLeft, FCenter, FRigth); (*Formatacao do Texto*)
type CTypeText = (TT_NORMAL, TT_DOUBLE, TT_BOLD, TT_UNDERLINE ); (*Tipo do Texto*)


  { Timp }

type TImp = class(tobject)
    private
      FDevice: string;
          LazSerial1: TSdpoSerial;

          FSetDevice : string;
          Ftipoimp : CTipoIMP;
          Fmodeloimp: CModeloIMP;
          FCOLUNA : integer;
          procedure SetDevice( value: string);
          procedure SetTipoimp(value : CTipoIMP);
          procedure Setmodeloimp(value : CModeloIMP);
    public
          constructor create(PLazSerial1 : TSdpoSerial);
          destructor destroy();
          procedure DefaultSerial();
          function FormatacaoString(info: string;tam: integer; Formatacao:CFormat; margin: integer): TStringList;
          procedure TextoSerial(info : string);
          procedure TextoSerial(info : string; Formatacao : CFormat);
          procedure TextoSerial(info : string; Formatacao : CFormat; typetext : CTypeText);
          procedure PrintBARCODE(info : string);
          procedure LineSerial();
          procedure close();
          procedure open;
          procedure Guilhotina();
          procedure Gaveta();
          procedure Beep();
          procedure EjetarCUPOM();
          property device : string read FDevice write SetDevice ;
          property tipoimp : CTipoIMP read Ftipoimp write SetTipoimp;
          property modeloimp : CModeloIMP read Fmodeloimp write SetModeloImp;
          property Coluna : integer read FCOLUNA write FCOLUNA;
  end;


var
  fImp: TImp;

implementation

procedure TImp.SetDevice(value: string);
begin
  FDevice := value;
  LazSerial1.device := FDevice;
end;

procedure TImp.SetTipoimp(value: CTipoIMP);
begin
  Ftipoimp:= value;
end;

procedure TImp.Setmodeloimp(value: CModeloIMP);
begin
  Fmodeloimp := value;
end;

constructor TImp.create(PLazSerial1 : TSdpoSerial);
begin
  LazSerial1:= PLazSerial1;
  DefaultSerial();
end;

destructor TImp.destroy();
begin

end;

procedure TImp.DefaultSerial();
begin

  LazSerial1.BaudRate:= br__9600;
  LazSerial1.DataBits:=db8bits;
  LazSerial1.FlowControl := fcNone;
  LazSerial1.StopBits:= sbOne;

end;


procedure TImp.TextoSerial(info: string; Formatacao: CFormat);
var
  info2 : Tstringlist;
  tam : integer;
  a : integer;
begin
  info := copy(info,1,FColuna);
  if modeloimp = MI_ELGINI9 then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       tam := impElginI9.Coluna;
       impElginI9.destroy();
  end;
  if modeloimp = MI_IMP59COL then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       tam := 59;
       impElginI9.Coluna := FCOLUNA;
       impElginI9.destroy();
  end;

  info2 := FormatacaoString(info, tam, Formatacao,4);
  for a := 0 to info2.Count-1 do
    TextoSerial(info2.Strings[a]);
end;

procedure TImp.TextoSerial(info: string; Formatacao: CFormat;
  typetext: CTypeText);
var
  info2 : Tstringlist;
  tam : integer;
  a : integer;
  typetextantes : string;
  typetextdepois : string;
begin
  info := copy(info,1,FColuna);
  if modeloimp = MI_ELGINI9 then
  begin
       impElginI9 := TIMP_ELGINI9.create();

       tam := impElginI9.Coluna;
       typetextdepois:= IMPELGINI9.Normal();
       if (typetext = TT_NORMAL) then
       begin
            typetextantes:= IMPELGINI9.Normal();
       end;

       if (typetext = TT_DOUBLE) then
       begin
            typetextantes:= IMPELGINI9.DoubleTexto();
            tam := (tam div 2);
       end;

       if (typetext = TT_BOLD) then
       begin
            typetextantes:= IMPELGINI9.Negrito();
       end;

       if (typetext = TT_UNDERLINE) then
       begin
            typetextantes:= IMPELGINI9.Sublinhado();
       end;

       impElginI9.destroy();
  end;
  if modeloimp = MI_IMP59COL then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       impElginI9.Coluna := FCOLUNA;
       tam := impElginI9.Coluna;
       typetextdepois:= IMPELGINI9.Normal();
       if (typetext = TT_NORMAL) then
       begin
            typetextantes:= IMPELGINI9.Normal();
       end;

       if (typetext = TT_DOUBLE) then
       begin
            typetextantes:= IMPELGINI9.DoubleTexto();
            tam := (tam div 2);
       end;

       if (typetext = TT_BOLD) then
       begin
            typetextantes:= IMPELGINI9.Negrito();
       end;

       if (typetext = TT_UNDERLINE) then
       begin
            typetextantes:= IMPELGINI9.Sublinhado();
       end;

       impElginI9.destroy();
  end;

  info2 := FormatacaoString(info, tam, Formatacao,iif(typetext=TT_DOUBLE,2,4));
  TextoSerial(typetextantes);
  for a := 0 to info2.Count-1 do
    TextoSerial(info2.Strings[a]);
  TextoSerial(typetextdepois);
end;

procedure TImp.PrintBARCODE(info: string);
var
  impElginI9 : TIMP_ELGINI9;
  tmp : string;
begin
  if modeloimp = MI_ELGINI9 then
  begin
       impElginI9 := TIMP_ELGINI9.create();

       tmp := impElginI9.Barra1D(info,$1D,$48,$2);
       impElginI9.destroy();
  end;
  if modeloimp = MI_IMP59COL then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       impElginI9.Coluna := FCOLUNA;
       tmp := impElginI9.Barra1D(info,$1D,$48,$2);
       impElginI9.destroy();
  end;
  LazSerial1.WriteData(tmp);
  //frmtoolprinter.CallCMD('BARCODE:'+info);
  //TextoSerial(info,FCenter,TT_BOLD);

end;

procedure TImp.TextoSerial(info: string);
var
  impElginI9 : TIMP_ELGINI9;
  tmp : string;
begin
  if modeloimp = MI_ELGINI9 then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       tmp := impElginI9.LineText(info);
       impElginI9.destroy();
  end;
  if modeloimp = MI_IMP59COL then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       impElginI9.Coluna := FCOLUNA;
       tmp := impElginI9.LineText(info);
       impElginI9.destroy();
  end;
  LazSerial1.WriteData(tmp);
  //frmtoolprinter.CallCMD('TEXT:'+tmp);
end;

procedure TImp.EjetarCUPOM();
begin
  if modeloimp = MI_ELGINI9 then
  begin
    TextoSerial(' -------- ',FCenter);
    //LineSerial();
    LineSerial();
    LineSerial();
    LineSerial();
    LineSerial();
    LineSerial();
    LineSerial();
    LineSerial();
  end;
   if modeloimp = MI_IMP59COL then
  begin
    LineSerial();
  end;
end;

procedure TImp.LineSerial();
var
  impElginI9 : TIMP_ELGINI9;
  tmp : string;

begin
  if modeloimp = MI_ELGINI9 then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       tmp := impElginI9.NewLine();
       impElginI9.free();
  end;
  if modeloimp = MI_IMP59COL then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       impElginI9.Coluna := FCOLUNA;
       tmp := impElginI9.NewLine();
       impElginI9.free();
  end;
  LazSerial1.WriteData(tmp);
  //frmtoolprinter.CallCMD('TEXT:'+tmp);
  //frmcupom.mecupom.Lines.Append('');
  sleep(200);
end;

procedure TImp.close();
begin
  LazSerial1.close();
end;

procedure TImp.open;
begin
  LazSerial1.open;
end;

procedure TImp.Guilhotina();
var

  impElginI9 : TIMP_ELGINI9;
  tmp : string;

begin

  if modeloimp = MI_ELGINI9 then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       tmp := impElginI9.Guilhotina();
       impElginI9.free();
  end;
  if modeloimp = MI_IMP59COL then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       impElginI9.Coluna := FCOLUNA;
       tmp := impElginI9.Guilhotina();
       impElginI9.free();
  end;

  LazSerial1.WriteData(tmp);
  //frmtoolprinter.CallCMD('PICCUT:FULL');
  //frmcupom.mecupom.Lines.Append('');
  sleep(200);
end;

procedure TImp.Gaveta();
var
  impElginI9 : TIMP_ELGINI9;
  tmp : string;
begin
  if modeloimp = MI_ELGINI9 then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       tmp := impElginI9.AcionaGaveta();
       impElginI9.free();
  end;
  if modeloimp = MI_IMP59COL then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       impElginI9.Coluna := FCOLUNA;
       tmp := impElginI9.AcionaGaveta();
       impElginI9.free();
  end;

  LazSerial1.WriteData(tmp);
  //frmtoolprinter.CallCMD('TEXT:'+tmp);
  //frmcupom.mecupom.Lines.Append('');
  sleep(200);
end;

procedure TImp.Beep();
var
  impElginI9 : TIMP_ELGINI9;
  tmp : string;
begin
  if modeloimp = MI_ELGINI9 then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       tmp := impElginI9.beep();
       impElginI9.free();
  end;
  if modeloimp = MI_IMP59COL then
  begin
       impElginI9 := TIMP_ELGINI9.create();
       impElginI9.Coluna := FCOLUNA;
       tmp := impElginI9.beep();
       impElginI9.free();
  end;

  LazSerial1.WriteData(tmp);
  //frmtoolprinter.CallCMD('TEXT:'+tmp);
  //frmcupom.mecupom.Lines.Append('');
  sleep(200);
end;

function TImp.FormatacaoString(info: string; tam: integer; Formatacao: CFormat;
  margin: integer): TStringList;
var
  tam2 : integer;
  listagem : TStringList;
  margem : integer;
  aux : string;
  spaces : string;
  margemesquerda : integer;
begin
  listagem := TStringList.create();
  margem := margin;
  (*Quebrando em linhas*)
  repeat
           aux := copy(info,0,(tam-margem));
           if (Formatacao = FLeft) then
           begin
                spaces := space(margem div 2);
           end;
           if (Formatacao = FCENTER) then
           begin
                margemesquerda := margem div 2;
                spaces := space(margemesquerda)+space((Length(aux) div 2)-margemesquerda);
           end;
           if (Formatacao = FRigth) then
           begin
                margemesquerda := margem div 2;
                spaces := space(tam-Length(aux));
           end;

           aux :=spaces + aux;
           //showmessage(aux);
           listagem.Append(aux );
           //frmcupom.mecupom.Lines.Append(aux);
           info := copy(info,(tam-margem)+1,Length(info));
  until (Length(info)<(tam-margem)) ;

  (*Formatando linhas*)
  result := listagem;
end;

end.

