 /*=============================================================================

   PIC Bootloader +

   Original bootloader was made by Petr Kolomaznik (Czech Republic).
   http://www.ehl.cz/pic, email: kolomaznik@ehl.cz.

   This software is based on the Delphi sources (1.0.8.0  25.7.2001)
   provided by (C)2000-2001 EHL elektronika.

   The Delphi source from EHL was converted to Borland Builder C.
   The serial console was added by Herman Aartsen, TNO - NL
   Send comments to : HermanAartsen@yahoo.co.uk.

   Version history Bootloader+ :

   V1.01 - Initial version
   V1.02 - Version tested on W98, NT4 and XP

   Known problems v1.02

   - console does not receive characters immediatly after bootload.

 ==============================================================================*/

#include <vcl.h>
#include <vcl\inifiles.hpp>
#include <stdio.h>

#pragma hdrstop

#include "Unit1.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

#define SMALLFORMHEIGHT 195	// height of form during bootload
#define LARGEFORMHEIGHT 523	// normal height of form

#define ROWS 12 		// number of colums in terminal window
#define COLS 80		// number of rows in terminal window

#define READ	0xE0
#define RACK	0xE1

#define WRITE	0xE3
#define WOK		0xE4
#define WBAD	0xE5

#define DATA_OK	0xE7
#define DAT_BAD	0xE8

#define IDENT	0xEA
#define IDACK	0xEB

#define DONE	0xED

#define TERMINBUFSIZE 100 // size of inputbuffer in terminal mode

typedef struct
{
   AnsiString LastFile;
   AnsiString Port;
   AnsiString BootSpeed;
   AnsiString TermSpeed;
   AnsiString Eeprom;
   int Echo;
   int CRLFin;
   int CRLFout;
} TSetup;

TSetup Setup;

TReadBase *ReadThread;	// Thread for terminal function

HANDLE hComm = NULL;		// Serail comm handle

AnsiString Directory;

bool	GoProgram;
int	CancelStart;

TIniFile *IniFile;

TForm1 *Form1;

__fastcall TForm1::TForm1(TComponent* Owner)
   : TForm(Owner)
{
   hStr[1] = 0;
   iCarPos = 0;
   iLines = 1;
   Memo1->Clear();
   Memo1->SelStart = Form1->Memo1->Perform(EM_LINEINDEX, 0, 0);
}

/*=============================================================================
   Opens selected COM port
=============================================================================*/
bool TForm1::OpenCom()
{
   bool fSuccess;
   DCB dcbCommPort;
   char* SerLinka;

   SerLinka = Setup.Port.c_str();

   COMMTIMEOUTS TimeOuts;

   hComm = CreateFile(SerLinka,        //open port
   GENERIC_READ | GENERIC_WRITE,
   0,                                  //exclusive access
   0,                                //no security attrs
   OPEN_EXISTING,
   FILE_ATTRIBUTE_NORMAL,
   0
   );

   if(hComm == INVALID_HANDLE_VALUE)
   {
      Application->MessageBox("Open port error !" , NULL, MB_OK);
      return(false);
   }

   //set of parameters
   fSuccess = GetCommState(hComm, &dcbCommPort);
   if(!fSuccess)
   {
      Application->MessageBox("Read port parameters error !", NULL, MB_OK);
      return(false);
   }

   dcbCommPort.BaudRate = Setup.TermSpeed.ToIntDef(19200);
   dcbCommPort.ByteSize = 8;
   dcbCommPort.Parity = NOPARITY;
   dcbCommPort.StopBits = ONESTOPBIT;
   dcbCommPort.fBinary = 1;

   fSuccess = SetCommState(hComm, &dcbCommPort);    //write of parameters back

   if(!fSuccess)
   {
      Application->MessageBox("Write port parameters error !", NULL, MB_OK);
      return(false);
   }

   // Set timeouts of communication port short. Setting high timeout
   // introduces problems for console mode.
   // ReadFile seems to block outgoing communication

   TimeOuts.ReadIntervalTimeout = 50;
   TimeOuts.ReadTotalTimeoutMultiplier = 0;
   TimeOuts.ReadTotalTimeoutConstant = 10;

   TimeOuts.WriteTotalTimeoutMultiplier = 20;
   TimeOuts.WriteTotalTimeoutConstant = 1000;

   fSuccess = SetCommTimeouts(hComm,&TimeOuts);

   if(!fSuccess)
   {
      Application->MessageBox("Set communication timeout error !", NULL, MB_OK);
      return(false);
   }
   //clear buffers
   fSuccess = PurgeComm(hComm,PURGE_TXABORT+PURGE_RXABORT+PURGE_TXCLEAR+PURGE_RXCLEAR);
   if(!fSuccess)
   {
      Application->MessageBox("Clear buffers error !", NULL, MB_OK);
      return(false);
   }
   return(true);
}

/*=============================================================================
   Change baudrate of already open COM port
=============================================================================*/
bool TForm1::ChangeComSpeed(int iSpeed)
{
   bool fSuccess;
   DCB dcbCommPort;

   // Set of parameters
   fSuccess = GetCommState(hComm, &dcbCommPort);
   if(!fSuccess)
   {
      Application->MessageBox("Read port parameters error !", NULL, MB_OK);
      return(false);
   }

   dcbCommPort.BaudRate = iSpeed;

   fSuccess = SetCommState(hComm, &dcbCommPort);    //write of parameters back

   if(!fSuccess)
   {
      Application->MessageBox("Write port parameters error !", NULL, MB_OK);
      return(false);
   }

   fSuccess = PurgeComm(hComm,PURGE_TXABORT+PURGE_RXABORT+PURGE_TXCLEAR+PURGE_RXCLEAR);

   if(!fSuccess)
   {
      Application->MessageBox("Clear buffers error !", NULL, MB_OK);
      return(false);
   }
   return(true);
}

/*=============================================================================
   Create form and read .INI for presets
=============================================================================*/
void __fastcall TForm1::FormCreate(TObject *Sender)
{
   TStringList* IniValueList;
   AnsiString aStr;

   int I,Code;
   AnsiString S;

   SpeedButton3->Enabled = true;

   Directory = GetCurrentDir();
   if (Directory.Length() > 3) Directory = Directory + "\\";

   IniFile = new TIniFile(Directory + "BootPlus.ini");
   Setup.LastFile = IniFile->ReadString("Setup", "File", "c:");
   Setup.Port = IniFile->ReadString("Setup", "Port", "COM1");
   Setup.BootSpeed = IniFile->ReadString("Setup", "BootSpeed", "19200");
   Setup.Eeprom = IniFile->ReadString("Setup", "Eeprom", "0");
   Setup.TermSpeed = IniFile->ReadString("Setup", "TermSpeed", "9600");

   aStr = IniFile->ReadString("Setup", "Echo", "0");
   if(aStr=="1") Setup.Echo = 1; else Setup.Echo = 0;

   aStr = IniFile->ReadString("Setup", "CRLFin", "0");
   if(aStr=="1") Setup.CRLFin = 1; else Setup.CRLFin = 0;

   aStr = IniFile->ReadString("Setup", "CRLFout", "0");
   if(aStr=="1") Setup.CRLFout = 1; else Setup.CRLFout = 0;

   S = ParamStr(1);
   if (S != "") Setup.LastFile = S;
   Edit1->Text = ExtractFileName(Setup.LastFile);

   if (Setup.Port == "COM1")
   {
      ComboBox2->ItemIndex = 0;
   }
   else
   {
      if (Setup.Port == "COM2")
      {
         ComboBox2->ItemIndex = 1;
      }
      else
      {
         if (Setup.Port == "COM3")
         {
            ComboBox2->ItemIndex = 2;
         }
         else
         {
            if (Setup.Port == "COM4")
            {
               ComboBox2->ItemIndex = 3;
            }
            else
            {
               ComboBox2->ItemIndex = 0;
               Setup.Port = "COM1";
            }
         }
      }
   }

   if (Setup.TermSpeed == "2400")
   {
      ComboBox3->ItemIndex = 0;
   }
   else
   {
      if (Setup.TermSpeed == "4800")
      {
         ComboBox3->ItemIndex = 1;
      }
      else
      {
         if (Setup.TermSpeed == "9600")
         {
            ComboBox3->ItemIndex = 2;
         }
         else
         {
            if (Setup.TermSpeed == "19200")
            {
               ComboBox3->ItemIndex = 3;
            }
            else
            {
               if (Setup.TermSpeed == "38400")
               {
                  ComboBox3->ItemIndex = 4;
               }
               else
               {
                  if (Setup.TermSpeed == "56000")
                  {
                     ComboBox3->ItemIndex = 5;
                  }
                  else
                  {
                     Setup.TermSpeed = "19200";
                     ComboBox3->ItemIndex = 3;
                  }
               }
            }
         }
      }
   }

   if (Setup.BootSpeed == "2400")
   {
      ComboBox1->ItemIndex = 0;
   }
   else
   {
      if (Setup.BootSpeed == "4800")
      {
         ComboBox1->ItemIndex = 1;
      }
      else
      {
         if (Setup.BootSpeed == "9600")
         {
            ComboBox1->ItemIndex = 2;
         }
         else
         {
            if (Setup.BootSpeed == "19200")
            {
               ComboBox1->ItemIndex = 3;
            }
            else
            {
               if (Setup.BootSpeed == "38400")
               {
                  ComboBox1->ItemIndex = 4;
               }
               else
               {
                  if (Setup.BootSpeed == "56000")
                  {
                     ComboBox1->ItemIndex = 5;
                  }
                  else
                  {
                     Setup.BootSpeed = "19200";
                     ComboBox1->ItemIndex = 3;
                  }
               }
            }
         }
      }
   }

   if (Setup.Eeprom == "1")
   {
      CheckBox1->Checked = true;
   }
   else
   {
      CheckBox1->Checked = false;
   }

   if (Setup.Echo)
   {
      CheckBox2->Checked = true;
   }
   else
   {
      CheckBox2->Checked = false;
   }

   if (Setup.CRLFin)
   {
      CheckBox3->Checked = true;
   }
   else
   {
      CheckBox3->Checked = false;
   }

   if (Setup.CRLFout)
   {
      CheckBox4->Checked = true;
   }
   else
   {
      CheckBox4->Checked = false;
   }

   if (FileExists(Setup.LastFile))
   {
      GoProgram = true;
   }
   else
   {
      GoProgram = false;
   }

   OpenCom();
   ReadThread = new TReadBase(false);
}

/*=============================================================================
   Close form and write setting to .INI file
=============================================================================*/
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
   IniFile->WriteString("Setup", "File", Setup.LastFile);
   IniFile->WriteString("Setup", "Port", Setup.Port);
   IniFile->WriteString("Setup", "BootSpeed", Setup.BootSpeed);
   IniFile->WriteString("Setup", "Eeprom", Setup.Eeprom);
   IniFile->WriteString("Setup", "TermSpeed", Setup.TermSpeed);

   if(Setup.Echo) IniFile->WriteString("Setup", "Echo", "1");
   else IniFile->WriteString("Setup", "Echo", "0");

   if(Setup.CRLFin) IniFile->WriteString("Setup", "CRLFin", "1");
   else IniFile->WriteString("Setup", "CRLFin", "0");

   if(Setup.CRLFout) IniFile->WriteString("Setup", "CRLFout", "1");
   else IniFile->WriteString("Setup", "CRLFout", "0");

   delete IniFile;

   CancelStart = 2;

   if(ReadThread->bRunning)
   {
      ReadThread->Terminate();
      ReadThread->WaitFor();
   }
   // Close COM port
   CloseHandle(hComm);
}

/*=============================================================================
   Search File dialog
=============================================================================*/
void __fastcall TForm1::ComboBox1Change(TObject *Sender)
{
  CancelStart = 1;
  Setup.BootSpeed = ComboBox1->Text;
}

/*=============================================================================
   Change COM port
=============================================================================*/
void __fastcall TForm1::ComboBox2Change(TObject *Sender)
{
   if(Setup.Port != ComboBox2->Text)
   {
      CancelStart = 1;
      Setup.Port = ComboBox2->Text;
      ReadThread->Terminate();
      ReadThread->WaitFor();
      // Close COM port
      CloseHandle(hComm);
      OpenCom();
      ReadThread = new TReadBase(false);
   }
}

/*=============================================================================
   Change terminal speed
=============================================================================*/
void __fastcall TForm1::ComboBox3Change(TObject *Sender)
{
   CancelStart = 1;
   Setup.TermSpeed = ComboBox3->Text;
   ChangeComSpeed(Setup.TermSpeed.ToIntDef(9600));
}

/*=============================================================================
=============================================================================*/
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  Timer1->Enabled = false;
}

/*=============================================================================
   Programming procedure
=============================================================================*/
void TForm1::Program (AnsiString Jmeno_Souboru, int System, int Port)
{
   AnsiString aStr;

   char* Data;
   bool ComOK, EndOfRecord;
   int NumberOfLines, LineNumber;
   DWORD Received;
   DWORD Sended;
   unsigned char RecBuff[2];
   unsigned char SendBuff[2];
   bool AutoStart;
   COMMTIMEOUTS TimeOuts;



   TStringList *TempList = new TStringList;

   // Disable all except Cancel button
   Form1->Height = SMALLFORMHEIGHT;
   SpeedButton3->Enabled = false;
   SpeedButton4->Enabled = false;
   ComboBox1->Enabled = false;
   ComboBox2->Enabled = false;
   CheckBox1->Enabled = false;

   ReadThread->Terminate();
   ReadThread->WaitFor();

   ProgressBar1->Position = 0;

   EndOfRecord = false;

   if (ChangeComSpeed(Setup.BootSpeed.ToIntDef(19200)) == true)
   {
      EscapeCommFunction(hComm, SETDTR);        // trigger pin = 0
      Edit2->Text = "Reset";

      EscapeCommFunction(hComm, SETRTS);        // Reset = 0
      Timer1->Enabled = true;
      while (Timer1->Enabled)
      {
         Application->ProcessMessages();
      }

      Application->ProcessMessages();
      EscapeCommFunction(hComm, CLRRTS);        // Reset = 1

      GetCommTimeouts(hComm,&TimeOuts);
      TimeOuts.ReadIntervalTimeout = 0;
      TimeOuts.ReadTotalTimeoutMultiplier = 1;
      TimeOuts.ReadTotalTimeoutConstant = 100;
      SetCommTimeouts(hComm,&TimeOuts);

      Edit2->Text = "Searching for bootloader.";

      AutoStart = false;
      CancelStart = 0;
      ComOK = false;
      while ((AutoStart == false) && (CancelStart == 0))
      {
         Application->ProcessMessages();
         PurgeComm(hComm,PURGE_TXABORT+PURGE_RXABORT+PURGE_TXCLEAR+PURGE_RXCLEAR);
         SendBuff[0] = IDENT;
         WriteFile(hComm, SendBuff, 1, &Sended, NULL);   //send IDENT
         ReadFile(hComm, RecBuff, 1, &Received, NULL);   //receive IDACK
         if ((Received == 1) && (RecBuff[0] == IDACK)) AutoStart = true;
      }

      TimeOuts.ReadIntervalTimeout = 50;
      TimeOuts.ReadTotalTimeoutMultiplier = 100;
      TimeOuts.ReadTotalTimeoutConstant = 10;
      SetCommTimeouts(hComm,&TimeOuts);

      if (AutoStart && (!CancelStart)) ComOK = true;

      if (ComOK)
      {
         Edit2->Text = "Writing, please wait !";

         TempList->LoadFromFile(Jmeno_Souboru);

         NumberOfLines = TempList->Count;
         if (NumberOfLines==0) ComOK = false;
         LineNumber = 0;

         while (ComOK && !EndOfRecord)
         {
            if (CancelStart) ComOK = false;

            aStr = TempList->Strings[LineNumber];
            LineNumber++;
            ProgressBar1->Position = (LineNumber*100) / NumberOfLines;

            if (aStr.Length() != 0)
            {
               Data = aStr.c_str();

               if (Data[0] == ':')
               {
                  if ((Data [7] == '0') && (Data [8] == '0'))
                  {
                     if (!Communication (WRITE, Data)) ComOK = false;
                  }
                  else
                  {
                     if ((Data [7] == '0') && (Data [8] == '1'))
                     {
                        EndOfRecord = true;          // End of File Record
                     }
                  }
               }
               else
               {
                  ComOK = false;
                  Application->MessageBox("Hex file error !", NULL, MB_OK);
               }
            }
            else
            {
               ComOK = false;
               Application->MessageBox(PChar("Hex file error : Empty line"), NULL, MB_OK);
            }
         }
      }

      if (ComOK == true)
      {
         if (Communication (DONE, Data))
         {
            ProgressBar1->Position = 100;
            Edit2->Text = "All OK !";
            EscapeCommFunction(hComm, CLRDTR);        // trigger pin = 1
            EscapeCommFunction(hComm, SETRTS);        // Reset = 0
            Timer1->Enabled = true;
            while (Timer1->Enabled)
            {
               Application->ProcessMessages();
            }
            EscapeCommFunction(hComm, CLRRTS);        // Reset = 1
         }
         else
         {
            ComOK = false;
         }

         if (CancelStart)
         {
            Edit2->Text = "Cancel of writing !";
         }
         else
         {
            if (!ComOK)
            {
               Edit2->Text = "Wrong writing !";
               Application->MessageBox("Writing error !", NULL, MB_OK);
            }
         }
         EscapeCommFunction(hComm, CLRDTR);        // trigger pin = 1
      }
      else
      {
         if (!CancelStart)
         {
            Edit2->Text = "Timeout of communication !";
            Application->MessageBox("Timeout of communication", NULL, MB_OK);
         }
         else
         {
            Edit2->Text = "Cancel of searching for bootloader.";
         }
      }
   }

   TimeOuts.ReadIntervalTimeout = 50;
   TimeOuts.ReadTotalTimeoutMultiplier = 0;
   TimeOuts.ReadTotalTimeoutConstant = 10;
   SetCommTimeouts(hComm,&TimeOuts);

   if(CancelStart!=2)	// not closing form
   {
      SpeedButton3->Enabled = true;
      ChangeComSpeed(Setup.TermSpeed.ToIntDef(9600));
      ReadThread = new TReadBase(false);

      Form1->Height = LARGEFORMHEIGHT;
      SpeedButton3->Enabled = true;
      SpeedButton4->Enabled = true;
      ComboBox1->Enabled = true;
      ComboBox2->Enabled = true;
      CheckBox1->Enabled = true;
   }
}

/*=============================================================================
   Hex string -> byte conversion
=============================================================================*/
unsigned char TForm1::GetHex(unsigned char* cPtr)
{
   unsigned char ucHulp;

   if(cPtr[0]<='9') ucHulp = cPtr[0] - '0';
   else ucHulp = cPtr[0] - 'A' + 10;
   ucHulp = ucHulp << 4;

   if(cPtr[1]<='9') ucHulp = ucHulp + cPtr[1] - '0';
   else ucHulp = ucHulp + cPtr[1] - 'A' + 10;

   return ucHulp;
}


/*=============================================================================
   Communication routine called by programming procedure
=============================================================================*/
bool TForm1::Communication(unsigned char Instr, unsigned char* VysBuff)
{
   DWORD Sended;
   DWORD Received;
   unsigned char CheckSum;
   unsigned char NumberOfData, N, Pointer;
   unsigned char RecBuff[41];
   unsigned char SendBuff[41];
   unsigned char SendLength;
   unsigned char RecLength;
   int Code, I, J;
   bool bRetVal;
   bool fSuccess;
   unsigned int Address;

   fSuccess = true;

   SendBuff[0] = Instr;
   SendLength = 1;
   RecLength = 1;

   if (Instr == WRITE)
   {
      Address = GetHex(&VysBuff[3]);
      Address = Address << 8;
      Address = Address + GetHex(&VysBuff[5]);
      Address = Address >> 1;

      if ((Address >= 0x2000) && (Address < 0x2100))
      {
         //don't send address from 0x2000 to 0x20FF
         return(true);
      }

      if ((Address >= 0x2100) && (!CheckBox1->Checked))
      {
         return(true);
      }

      SendBuff[1] = (unsigned char)(Address >> 8);			//high byte of address
      SendBuff[2] = (unsigned char)(Address & 0x00FF);	//low byte of address

      NumberOfData = GetHex(&VysBuff[1]);

      SendBuff[3] = NumberOfData;                  //number of data
      CheckSum = 0;
      for (N=1;N<=NumberOfData/2;N++)
      {
         Pointer = (N-1) * 4;
         I = GetHex(&VysBuff[11+Pointer]);

         SendBuff [5 + ((N-1)*2)] = I;             //high byte of instruction
         CheckSum = CheckSum + I;

         I = GetHex(&VysBuff[9+Pointer]);
         SendBuff [6 + ((N-1)*2)] = I;             //low byte of instruction
         CheckSum += I;
      }
      SendBuff[4] = CheckSum;                     //checksum
      SendLength = 5 + NumberOfData;
      RecLength = 2;                              //wait for 2 bytes
   }

   Application->ProcessMessages();
   PurgeComm(hComm,PURGE_TXABORT+PURGE_RXABORT+PURGE_TXCLEAR+PURGE_RXCLEAR);
   WriteFile(hComm, SendBuff, SendLength, &Sended, NULL);  //send
   ReadFile(hComm, RecBuff, RecLength, &Received, NULL);   //receive

   if(Received > 0)
   {
      switch(Instr)
      {
         case IDENT:
            if (RecBuff[0] == IDACK) bRetVal = true;
            else bRetVal = false;
         break;

         case WRITE:
            if ((RecBuff[0] == DATA_OK) && (RecBuff[1] == WOK)) bRetVal = true;
            else bRetVal = false;
         break;

         case DONE:
            if (RecBuff[0] == WOK) bRetVal = true;
            else bRetVal = false;
         break;
      }
   }
   else fSuccess = false;

   PurgeComm(hComm,PURGE_TXABORT+PURGE_RXABORT+PURGE_TXCLEAR+PURGE_RXCLEAR);

   if(!fSuccess)
   {
      Application->MessageBox("Timeout of communication!", NULL, MB_OK);
      bRetVal = false;
   }
   return bRetVal;
}


/*=============================================================================
   Search .hex file dialog
=============================================================================*/
void __fastcall TForm1::FileSearch()
{
  CancelStart = 1;
  OpenDialog1->Filter = "Hex file (*.hex) | *.hex";
  OpenDialog1->InitialDir = ExtractFileDir (Setup.LastFile);
  OpenDialog1->FileName = ExtractFileName (Setup.LastFile);

  if(OpenDialog1->Execute())
  {
      try
      {
         GoProgram = true;
         Setup.LastFile = OpenDialog1->FileName;
         Edit1->Text = ExtractFileName (Setup.LastFile);
      }

      __except(1)
      {
         Application->MessageBox("File read error !", NULL, MB_OK);
      }

   }
}

/*=============================================================================
   Puts incoming characters to console
=============================================================================*/
void TForm1::ConsoleChar(unsigned char chRead)
{
   // filter problematic characters 
   if(chRead==0 || chRead==9) chRead = 254;

   switch(chRead)
   {
      case 10:
         Memo1->Lines->Append(" ");
         iLines = Memo1->Lines->Count;
         if(iLines > ROWS)
         {
            Memo1->Lines->Delete(0);
            iLines = ROWS;
         }
         iCarPos = 0;
         Memo1->SelStart = Memo1->Perform(EM_LINEINDEX, iLines-1, 0);
      break;

      case 13:
         if(Setup.CRLFin)
         {
            Memo1->Lines->Append(" ");
            iLines = Memo1->Lines->Count;
            if(iLines > ROWS)
            {
               Memo1->Lines->Delete(0);
               iLines = ROWS;
            }
         }

         iCarPos = 0;
         Memo1->SelStart = Memo1->Perform(EM_LINEINDEX, iLines-1, 0);
      break;

      default:
         Memo1->SelStart = Memo1->Perform(EM_LINEINDEX, iLines-1, 0) + iCarPos;
         Memo1->SelLength = 1;
         hStr[0] = chRead;
         Memo1->SelText = hStr;
         iCarPos++;
         if(iCarPos >= COLS)
         {
            Memo1->Lines->Append(" ");
            iLines = Memo1->Lines->Count;
            if(iLines > ROWS)
            {
               Memo1->Lines->Delete(0);
               iLines = ROWS;
            }
            iCarPos = 0;
            Memo1->SelStart = Memo1->Perform(EM_LINEINDEX, iLines-1, 0);
         }
      break;
   }
}

/*=============================================================================
   Called at creation of thread
=============================================================================*/
__fastcall TReadBase::TReadBase(bool CreateSuspended)
   : TThread(CreateSuspended)
{
   bRunning = True;
}

/*=============================================================================
   Thread for receiving characters from PIC applicaion
=============================================================================*/
void __fastcall TReadBase::Execute()
{

   DWORD dwCommEvent;
   DWORD dwRead;

   char  chReadBuffer[TERMINBUFSIZE];
   char* chReadPtr;
   int iHelp;

  // MAKE THE THREAD OBJECT AUTOMATICALLY DESTROYED WHEN THE THREAD TERMINATES.

   FreeOnTerminate = true; // automatically distroy thread when terminated

   while(!Terminated)
   {
      // Sleep() a while allowing time for TransmitCommChar() and other events
      Sleep(8);

      ReadFile(hComm, chReadBuffer, TERMINBUFSIZE, &dwRead, NULL);

      chReadPtr = chReadBuffer;
      while(dwRead) // character received
      {
         Form1->ConsoleChar(*chReadPtr);
         dwRead--;
         chReadPtr++;
      }
   } // if(dwRead) character received

   bRunning = False;
}



/*=============================================================================
   Executes when key is pressed
=============================================================================*/
void __fastcall TForm1::FormKeyPress(TObject *Sender, char &Key)
{
   AnsiString Astr;
   int iHelp;

   if(ReadThread->bRunning)
   {
      // Clear any left data data in buffer
      PurgeComm(hComm, PURGE_RXCLEAR);
      // Transmit character
      TransmitCommChar(hComm, Key);
      if(Setup.CRLFout && Key==13) TransmitCommChar(hComm, 10);
      if(Setup.Echo)	Form1->ConsoleChar(Key);
   }
}


/*=============================================================================
=============================================================================*/
void __fastcall TForm1::CheckBox2Click(TObject *Sender)
{
   if(CheckBox2->Checked) Setup.Echo = 1; else Setup.Echo = 0;
}

/*=============================================================================
=============================================================================*/
void __fastcall TForm1::CheckBox3Click(TObject *Sender)
{
   if(CheckBox3->Checked) Setup.CRLFin = 1; else Setup.CRLFin = 0;
}

/*=============================================================================
=============================================================================*/
void __fastcall TForm1::CheckBox4Click(TObject *Sender)
{
   if(CheckBox4->Checked) Setup.CRLFout = 1; else Setup.CRLFout = 0;
}

/*=============================================================================
=============================================================================*/
void __fastcall TForm1::ComboBox1KeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
   Key = 0;
}


/*=============================================================================
=============================================================================*/
void __fastcall TForm1::SpeedButton2Click(TObject *Sender)
{
  CancelStart = 1;
}

/*=============================================================================
=============================================================================*/
void __fastcall TForm1::SpeedButton3Click(TObject *Sender)
{
  if (GoProgram) Program (Setup.LastFile, 1, 1);
  else Application->MessageBox("No hex file specified or file does not exist !", NULL, MB_OK);
}

/*=============================================================================
   Clear console
=============================================================================*/
void __fastcall TForm1::SpeedButton1Click(TObject *Sender)
{
   hStr[1] = 0;
   iCarPos = 0;
   iLines = 1;
   Memo1->Clear();
   Memo1->SelStart = Form1->Memo1->Perform(EM_LINEINDEX, 0, 0);
}

/*=============================================================================
   File search
=============================================================================*/
void __fastcall TForm1::SpeedButton4Click(TObject *Sender)
{
   FileSearch();
}


