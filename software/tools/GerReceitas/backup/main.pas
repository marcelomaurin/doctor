unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, SynEdit, SynCompletion, SynHighlighterPas,
  SynMemo, TAChartExtentLink, TAGraph, TASeries, AdvLed, indGnouMeter,
  LedNumber, Sensors, uPSComponent, SdpoSerial, Forms, Controls, Graphics,
  Dialogs, ExtCtrls, ComCtrls, StdCtrls, types;

type

  { TfrmMain }

  TfrmMain = class(TForm)
    Button1: TButton;
    btSOUND: TButton;
    edSrv: TEdit;
    edFirmware: TEdit;
    Label18: TLabel;
    Label19: TLabel;
    Label20: TLabel;
    Label21: TLabel;
    Label22: TLabel;
    ledRele1: TAdvLed;
    ledRele2: TAdvLed;
    ledRele3: TAdvLed;
    ledRele4: TAdvLed;
    AnalogSensor1: TAnalogSensor;
    BTBEEP: TButton;
    btMedir: TButton;
    btCarregar: TButton;
    btGravar: TButton;
    BTMSGSTOP: TButton;
    btRele3OFF: TButton;
    btRele4OFF: TButton;
    btRele2OFF: TButton;
    btRele5: TButton;
    btUSBCarregar: TButton;
    BTMAN: TButton;
    btNovo: TButton;
    btRele1: TButton;
    btRele2: TButton;
    btRele3ON: TButton;
    btRele4: TButton;
    BTLSTDIR: TButton;
    BTMSG: TButton;
    btClear: TButton;
    btRUN: TButton;
    btTempMin: TButton;
    cbFiles: TComboBox;
    Chart1: TChart;
    Chart1LineSeries1: TLineSeries;
    edConsole: TEdit;
    edFile: TEdit;
    edTempMax: TEdit;
    edTempMin: TEdit;
    edTempo: TEdit;
    edMsg: TEdit;
    indGnouMeter1: TindGnouMeter;
    Label10: TLabel;
    Label11: TLabel;
    Label12: TLabel;
    Label13: TLabel;
    Label14: TLabel;
    Label15: TLabel;
    Label16: TLabel;
    Label17: TLabel;
    Label7: TLabel;
    Label8: TLabel;
    Label9: TLabel;
    ledCtrl: TAdvLed;
    btAtivar: TButton;
    btDesativar: TButton;
    edUSB: TEdit;
    Edit2: TEdit;
    edIP: TEdit;
    Label1: TLabel;
    lbVersao: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    ledflgLeituraBasica: TAdvLed;
    ledTemp: TLEDNumber;
    OpenDialog1: TOpenDialog;
    PageControl1: TPageControl;
    Panel1: TPanel;
    Panel2: TPanel;
    SaveDialog1: TSaveDialog;
    SdpoSerial1: TSdpoSerial;
    SynCompletion1: TSynCompletion;
    SynEdit1: TSynEdit;
    Console: TSynMemo;
    TabSheet1: TTabSheet;
    tbTermUSB: TTabSheet;
    tbBluetooth: TTabSheet;
    tbEthernet: TTabSheet;
    tbUSB: TTabSheet;
    tbSobre: TTabSheet;
    tbSetup: TTabSheet;
    tbScript: TTabSheet;
    Timer1: TTimer;
    procedure btAtivarClick(Sender: TObject);
    procedure BTBEEPClick(Sender: TObject);
    procedure btCarregarClick(Sender: TObject);
    procedure btClearClick(Sender: TObject);
    procedure btDesativarClick(Sender: TObject);
    procedure BTLSTDIRClick(Sender: TObject);
    procedure BTMANClick(Sender: TObject);
    procedure btMedirClick(Sender: TObject);
    procedure BTMSGClick(Sender: TObject);
    procedure BTMSGSTOPClick(Sender: TObject);
    procedure btNovoClick(Sender: TObject);
    procedure btRele2Click(Sender: TObject);
    procedure btRele2OFFClick(Sender: TObject);
    procedure btRele3ONClick(Sender: TObject);
    procedure btRele3OFFClick(Sender: TObject);
    procedure btRele4Click(Sender: TObject);
    procedure btRele4OFFClick(Sender: TObject);
    procedure btRele5Click(Sender: TObject);
    procedure btRUNClick(Sender: TObject);
    procedure btTempMinClick(Sender: TObject);
    procedure btUSBCarregarClick(Sender: TObject);
    procedure btGravarClick(Sender: TObject);
    procedure btRele1Click(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure btSOUNDClick(Sender: TObject);
    procedure edConsoleChange(Sender: TObject);
    procedure edConsoleKeyPress(Sender: TObject; var Key: char);
    procedure SdpoSerial1RxData(Sender: TObject);
    procedure tbUSBContextPopup(Sender: TObject; MousePos: TPoint;
      var Handled: Boolean);
    procedure Timer1Timer(Sender: TObject);
  private
    { private declarations }
    tempostart : double;
    arquivo : string; //Nome do Arquivo de Script
  public
    { public declarations }
    buffer: String;
    flgErro : boolean;

    procedure Analisa(Info : string);
  end;

var
  frmMain: TfrmMain;

implementation

{$R *.lfm}

{ TfrmMain }

procedure TfrmMain.btAtivarClick(Sender: TObject);
begin
  Console.Lines.Clear;
  Chart1LineSeries1.Clear;
  SdpoSerial1.Device := edUSB.Text;
  SdpoSerial1.Open;
  buffer := '';
  // TLedState = (lsDisabled,lsOff, lsOn);
  if SdpoSerial1.Active then
  begin
       ledCtrl.State:= lsOn;
  end
  else
  begin
    ledCtrl.State:= lsOff;
  end;
end;

procedure TfrmMain.BTBEEPClick(Sender: TObject);
begin
    if SdpoSerial1.Active then
  begin
       SdpoSerial1.WriteData('BEEP;'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.btCarregarClick(Sender: TObject);
begin
  OpenDialog1.InitialDir:= ExtractFileDir(Application.ExeName);
  if   OpenDialog1.Execute then
  begin
       arquivo := OpenDialog1.FileName;
       SynEdit1.Lines.LoadFromFile(arquivo);
       frmMain.Text := 'Gerenciador de Panela '+ arquivo;
  end;
end;

procedure TfrmMain.btClearClick(Sender: TObject);
begin
  cbFiles.Items.Clear;
end;

procedure TfrmMain.btDesativarClick(Sender: TObject);
begin
  SdpoSerial1.Close;
  if SdpoSerial1.Active then
  begin
       ledCtrl.State:= lsOn;

  end
  else
  begin
    ledCtrl.State:= lsOff;
  end;
end;

procedure TfrmMain.BTLSTDIRClick(Sender: TObject);
begin
  if SdpoSerial1.Active then
  begin
       cbFiles.Items.Clear;
       SdpoSerial1.WriteData('LSTDIR;'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.BTMANClick(Sender: TObject);
begin
  if SdpoSerial1.Active then
  begin
       SdpoSerial1.WriteData('MAN;'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;

end;

procedure TfrmMain.btMedirClick(Sender: TObject);
var
  myFormatSettings: TFormatSettings;
  Info :String;
begin
  if SdpoSerial1.Active then
  begin
       tempostart := now();
      SdpoSerial1.WriteData('TEMPERATURA;'+#10);
       myFormatSettings.DecimalSeparator := '.';
       AnalogSensor1.ValueMax:= 150;
       Info := edTempMax.text;
      AnalogSensor1.ValueRed := trunc(StrToFloat(Info,myFormatSettings));
      Info := edTempMin.text;
      //AnalogSensor1.ValueYellow := trunc(StrToFloat(Info,myFormatSettings));
      AnalogSensor1.ValueMin:= 0;
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.BTMSGClick(Sender: TObject);
begin
   if SdpoSerial1.Active then
  begin
       SdpoSerial1.WriteData('MSG='+edMsg.TEXT+';'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.BTMSGSTOPClick(Sender: TObject);
begin
   if SdpoSerial1.Active then
  begin
        SdpoSerial1.WriteData('MSGSTOP='+edMsg.TEXT+';'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.btNovoClick(Sender: TObject);
begin
  arquivo := '';
  frmMain.Text := 'Gerenciador de Panela '+ arquivo;
  SynEdit1.Lines.Clear;
end;

procedure TfrmMain.btRele2Click(Sender: TObject);
begin
   if SdpoSerial1.Active then
  begin
       SdpoSerial1.WriteData('BOMBA2=ON;'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.btRele2OFFClick(Sender: TObject);
begin
   if SdpoSerial1.Active then
  begin
       SdpoSerial1.WriteData('BOMBA2=OFF;'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.btRele3ONClick(Sender: TObject);
begin
   if SdpoSerial1.Active then
  begin
       SdpoSerial1.WriteData('RELE3=ON;'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.btRele3OFFClick(Sender: TObject);
begin
  if SdpoSerial1.Active then
  begin
       SdpoSerial1.WriteData('RELE3=OFF;'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;

end;

procedure TfrmMain.btRele4Click(Sender: TObject);
begin
   if SdpoSerial1.Active then
  begin
       SdpoSerial1.WriteData('RELE4=ON;'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.btRele4OFFClick(Sender: TObject);
begin
     if SdpoSerial1.Active then
  begin
       SdpoSerial1.WriteData('RELE4=OFF;'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;

end;

procedure TfrmMain.btRele5Click(Sender: TObject);
begin
    if SdpoSerial1.Active then
  begin
       SdpoSerial1.WriteData('BOMBA1=OFF;'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;

end;

procedure TfrmMain.btRUNClick(Sender: TObject);
begin
  if SdpoSerial1.Active then
  begin
       if (cbFiles.ItemIndex<>-1) then
       begin
            SdpoSerial1.WriteData('RUN('+cbFiles.Items[cbFiles.ItemIndex]+');'+#10);

       end;
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;

end;

procedure TfrmMain.btTempMinClick(Sender: TObject);
var
  info : string;
   myFormatSettings: TFormatSettings;
begin
  if SdpoSerial1.Active then
  begin
      tempostart := now();
      AnalogSensor1.ValueMax:= 150;
      AnalogSensor1.ValueMin:= 0;

      info := edTempMax.text;
      //GetLocaleFormatSettings(GetThreadLocale, myFormatSettings);
      myFormatSettings.DecimalSeparator := '.';

      AnalogSensor1.ValueRed := trunc(StrToFloat(Info,myFormatSettings));
      Info := edTempMin.text;
      //AnalogSensor1.ValueYellow := trunc(StrToFloat(Info,myFormatSettings));
      info := 'TEMPMIN='+edTempMax.text+','+edTempMin.text+','+edTempo.text+';'+#10;
       SdpoSerial1.WriteData(Info);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.btUSBCarregarClick(Sender: TObject);
var
a: integer;
begin
   if SdpoSerial1.Active then
  begin
        //OpenDialog1.InitialDir:= ExtractFileDir(Application.ExeName);
       OpenDialog1.InitialDir:= 'C:\Users\mmm\Documents\projetos\geison\fase 3\receitas';
        if   OpenDialog1.Execute then
        begin
             arquivo := OpenDialog1.FileName;
             SynEdit1.Lines.LoadFromFile(arquivo);
             flgErro := false;
             frmMain.Text := 'Gerenciador de Panela '+ arquivo;
             SdpoSerial1.WriteData('LOAD='+OpenDialog1.FileName+';'+#10);
             sleep(400);
             for a := 0 to SynEdit1.Lines.Count-1  do
             begin
               SdpoSerial1.WriteData( SynEdit1.Lines[a]+#13+#10);
               sleep(100);
               Application.ProcessMessages;
               sleep(100);
               if flgErro then
               begin
                 break;
               end;
             end;
             //SdpoSerial1.WriteData( '+'+#13+#10);
             SdpoSerial1.WriteData( '+');


       end;
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.btGravarClick(Sender: TObject);
begin
  SaveDialog1.InitialDir:= ExtractFileDir(Application.ExeName);
  if (arquivo = '') then
  begin
       if (SaveDialog1.Execute) then
       begin
            arquivo := SaveDialog1.FileName;
            SynEdit1.Lines.SaveToFile(arquivo);
       end;
  end
  else
  begin
    SynEdit1.Lines.SaveToFile(arquivo);
  end;
  frmMain.Text := 'Gerenciador de Panela '+ arquivo;

end;

procedure TfrmMain.btRele1Click(Sender: TObject);
begin
  if SdpoSerial1.Active then
  begin
       SdpoSerial1.WriteData('BOMBA1=ON;'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.Button1Click(Sender: TObject);
var
   a: integer;

begin
   if SdpoSerial1.Active then
  begin
             arquivo := edFile.Text;
             //SynEdit1.Lines.LoadFromFile(arquivo);
              flgErro := false;
             frmMain.Text := 'Gerenciador de Panela '+ arquivo;
             SdpoSerial1.WriteData('LOAD='+arquivo+';'+#10);
             sleep(400);
             for a := 0 to SynEdit1.Lines.Count-1  do
             begin
               SdpoSerial1.WriteData( SynEdit1.Lines[a]+#13+#10);
               sleep(100);
               Application.ProcessMessages;
               if flgErro then
               begin
                 break;
               end;
             end;
             //SdpoSerial1.WriteData( '+'+#13+#10);
             SdpoSerial1.WriteData( '+');

  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.btSOUNDClick(Sender: TObject);
begin
  if SdpoSerial1.Active then
  begin
       SdpoSerial1.WriteData('SOUND;'+#10);
  end
  else
  begin
    ShowMessage('Ative a porta USB!');
  end;
end;

procedure TfrmMain.edConsoleChange(Sender: TObject);
begin

end;

procedure TfrmMain.edConsoleKeyPress(Sender: TObject; var Key: char);
begin
   if SdpoSerial1.Active then
    begin
         if key = #13 then
         begin
              SdpoSerial1.WriteData(edConsole.Text+#10);
              edConsole.Text:= '';
         end;
    end
    else
    begin
      ShowMessage('Ative a porta USB!');
    end;
end;

//Realiza analise de comando
procedure TfrmMain.Analisa(Info : string);
var
  posicao, posicaofim,a : integer;
  Info2: String;
  myFormatSettings: TFormatSettings;
  Tempo : double;
begin
   posicao := pos('Atual:',Info);
   if (posicao > 0) then
   begin
        posicaofim := pos('C', Info);
        if (posicaofim>0) then
        begin
             Info := Copy(Info,posicao+7,posicaofim-(posicao+6));
             ledTemp.Caption:= Info;
             Info2 := Copy(Info,1,length(info)-1);
              //GetLocaleFormatSettings(GetThreadLocale, myFormatSettings);
              myFormatSettings.DecimalSeparator := '.';
             indGnouMeter1.Value:= StrToFloat(Info2,myFormatSettings);
             AnalogSensor1.Value:=StrToFloat(Info2,myFormatSettings);
             Tempo := (now()-tempostart)*24*60;
             Chart1LineSeries1.AddXY(Tempo, StrToFloat(Info2,myFormatSettings));
             //Chart1LineSeries2.AddXY(now(), getTempIdeal());
             //indGnouMeter1.Value:= StrToFloat(Info);

        end;
   end;
   //FILENAME:
   posicao := pos('FILENAME:',Info);
   if (posicao > 0) then
   begin
     posicaofim := pos(';', Info);
     if (posicaofim>0) then
     begin
          Info := Copy(Info,posicao+9,posicaofim-(posicao+9));
          cbFiles.Items.Append(Info);
          //indGnouMeter1.Value:= StrToFloat(Info);

     end;
   end;
   //RELE1:
   posicao := pos('RELE1:',Info);
   if (posicao > 0) then
   begin
     posicaofim := pos('ON;', Info);
     if (posicaofim>0) then
     begin
          //Info := Copy(Info,posicao+10,posicaofim-(posicao+10));
          //cbFiles.Items.Append(Info);
          //indGnouMeter1.Value:= StrToFloat(Info);
          ledRele1.State:= lsOff;

     end;
     posicaofim := pos('OFF;', Info);
     if (posicaofim>0) then
     begin
          //Info := Copy(Info,posicao+10,posicaofim-(posicao+10));
          //cbFiles.Items.Append(Info);
          //indGnouMeter1.Value:= StrToFloat(Info);
          ledRele1.State:= lsOn;
     end;

   end;
    //RELE2:
   posicao := pos('RELE2:',Info);
   if (posicao > 0) then
   begin
     posicaofim := pos('ON;', Info);
     if (posicaofim>0) then
     begin
          //Info := Copy(Info,posicao+10,posicaofim-(posicao+10));
          //cbFiles.Items.Append(Info);
          //indGnouMeter1.Value:= StrToFloat(Info);
          ledRele2.State:= lsOff;

     end;
     posicaofim := pos('OFF;', Info);
     if (posicaofim>0) then
     begin
          //Info := Copy(Info,posicao+10,posicaofim-(posicao+10));
          //cbFiles.Items.Append(Info);
          //indGnouMeter1.Value:= StrToFloat(Info);
          ledRele2.State:= lsOn;
     end;

   end;
    //RELE3:
   posicao := pos('RELE3:',Info);
   if (posicao > 0) then
   begin
     posicaofim := pos('ON;', Info);
     if (posicaofim>0) then
     begin
          //Info := Copy(Info,posicao+10,posicaofim-(posicao+10));
          //cbFiles.Items.Append(Info);
          //indGnouMeter1.Value:= StrToFloat(Info);
          ledRele3.State:= lsOff;

     end;
     posicaofim := pos('OFF;', Info);
     if (posicaofim>0) then
     begin
          //Info := Copy(Info,posicao+10,posicaofim-(posicao+10));
          //cbFiles.Items.Append(Info);
          //indGnouMeter1.Value:= StrToFloat(Info);
          ledRele3.State:= lsOn;
     end;

   end;
    //RELE4:
   posicao := pos('RELE4:',Info);
   if (posicao > 0) then
   begin
     posicaofim := pos('ON;', Info);
     if (posicaofim>0) then
     begin
          //Info := Copy(Info,posicao+10,posicaofim-(posicao+10));
          //cbFiles.Items.Append(Info);
          //indGnouMeter1.Value:= StrToFloat(Info);
          ledRele4.State:= lsOff;

     end;
     posicaofim := pos('OFF;', Info);
     if (posicaofim>0) then
     begin
          //Info := Copy(Info,posicao+10,posicaofim-(posicao+10));
          //cbFiles.Items.Append(Info);
          //indGnouMeter1.Value:= StrToFloat(Info);
          ledRele4.State:= lsOn;
     end;
   end;
   //Erro SD!
   posicao := pos('Erro SD', Info);
   if (posicao>0) then
   begin
          flgErro :=true;
          ShowMessage('Erro no SD!');
   end;
   //Recebendo Firmware:
   posicao := pos('Recebendo Firmware:', Info);
   if (posicao>0) then
   begin

          for a := 0 to SynEdit1.Lines.Count-1  do
          begin
            SdpoSerial1.WriteData( SynEdit1.Lines[a]+#10+#13);
            sleep(500);
            Application.ProcessMessages;
            sleep(500);
            if flgErro then
            begin
                 break;
            end;
          end;
          SdpoSerial1.WriteData( '+');
   end;
   //Erro no Script Linha:
   //Reconhecendo Erro
   posicao := pos('Erro no Script Linha:', Info);
   if (posicao>0) then
   begin
     posicao := posicao + 21;

     flgErro := true;
     posicaofim := pos(';', Info);
     if (posicaofim >0) then
     begin
       Info2 := copy(Info,posicao,posicaofim-posicao);
       //linha := strtoint(Info2);
       ShowMessage('Erro na Linha:'+Info);


     end;
   end;

   //Mostra status flag Stop
   posicao := pos('FlagLeituraBasica ativo!', Info);
   if (posicao>0) then
   begin
       ledflgLeituraBasica.Blink:=true;
   end;

   //Mostra status flag Stop
   posicao := pos('FlgLeituraBasica não Ativo!', Info);
   if (posicao>0) then
   begin
       ledflgLeituraBasica.Blink:=false;
   end;

   //Arquivo Salvo
   posicao := pos('Arquivo Salvo!', Info);
   if (posicao>0) then
   begin
       ShowMessage('Arquivo Salvo!');
   end;

   //IP:
   //Identificando IP
   posicao := pos('IP:', Info);
   if (posicao>0) then
   begin
       edIP.Text:= copy(Info,posicao+4,pos(#13,Info)-posicao+4);
   end;

   //Identificando Servico Ethernet
    posicao := pos('Ethernet Server Porta:', Info);
    if (posicao>0) then
    begin
        edSrv.Text:= copy(Info,posicao+22,pos(#13,Info)-posicao+22);
    end;

    //Comando Vazio!
   //Identifica Comando Vazio
    posicao := pos('Comando Vazio!', Info);
    if (posicao>0) then
    begin
        //edSrv.Text:= copy(Info,posicao+22,pos(#13,Info)-posicao+22);
    end;

    //Erro no Script Linha:
    posicao := pos('Erro no Script Linha:', Info);
    if (posicao>0) then
    begin
        //edSrv.Text:= copy(Info,posicao+22,pos(#13,Info)-posicao+22);
        Showmessage(Info);
    end;
    //Versao:0.M
    posicao := pos('Versao:', Info);
    if (posicao>0) then
    begin
        edFirmware.Text:= copy(Info,posicao+7,pos(#13,Info)-(posicao+7));
        if (edFirmware.Text <> lbVersao.Caption) then
        begin
            Showmessage('Versão Incorreta do Firmware');
            //Application.Terminate;
        end;
    end;


end;

procedure TfrmMain.SdpoSerial1RxData(Sender: TObject);
var
  Info : String;
  posfim : integer;
  comando : String;
begin
    Info := SdpoSerial1.ReadData;
    buffer:= buffer+Info;

    while(true)  do
    begin
       posfim := pos(#10,buffer);
         //Verifica se tem fim de linha
         if (posfim >0) then
         begin
              comando := copy(buffer,1,posfim);
              Analisa(comando);
              Console.Lines.Append(comando);
              (*
              if(posfim = length(buffer)) then
              begin
                   buffer := '';
              end
              else
              begin
              *)
                   buffer := Copy(buffer,posfim+1, (length(buffer)-posfim));
              //end;
         end;
         if (posfim <=0) then
         begin

              break;
         end;
    end;
end;

procedure TfrmMain.tbUSBContextPopup(Sender: TObject; MousePos: TPoint;
  var Handled: Boolean);
begin

end;

procedure TfrmMain.Timer1Timer(Sender: TObject);
begin
  ShowMessage('Versão não licenciada!');
end;

end.
