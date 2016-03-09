//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
   TLabel *Label2;
   TLabel *Label3;
   TLabel *Label5;
   TComboBox *ComboBox2;
   TEdit *Edit2;
   TProgressBar *ProgressBar1;
   TComboBox *ComboBox1;
   TCheckBox *CheckBox1;
   TCheckBox *CheckBox2;
   TCheckBox *CheckBox3;
   TCheckBox *CheckBox4;
   TOpenDialog *OpenDialog1;
   TTimer *Timer1;
   TComboBox *ComboBox3;
   TMemo *Memo1;
   TLabel *Label1;
   TLabel *Label4;
   TBevel *Bevel1;
   TSpeedButton *SpeedButton2;
   TSpeedButton *SpeedButton3;
   TLabel *ProductName;
   TEdit *Edit1;
   TSpeedButton *SpeedButton1;
   TSpeedButton *SpeedButton4;
   void __fastcall Timer1Timer(TObject *Sender);
   void __fastcall FileSearch();
   void __fastcall ComboBox2Change(TObject *Sender);
   void __fastcall ComboBox1Change(TObject *Sender);
   void __fastcall FormCreate(TObject *Sender);
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
   void __fastcall ComboBox3Change(TObject *Sender);
   void __fastcall FormKeyPress(TObject *Sender, char &Key);
   void __fastcall CheckBox2Click(TObject *Sender);
   void __fastcall CheckBox3Click(TObject *Sender);
   void __fastcall CheckBox4Click(TObject *Sender);
   void __fastcall ComboBox1KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
   void __fastcall SpeedButton2Click(TObject *Sender);
   void __fastcall SpeedButton3Click(TObject *Sender);
   void __fastcall SpeedButton1Click(TObject *Sender);
   void __fastcall SpeedButton4Click(TObject *Sender);
private:	// User declarations
   int iCarPos;
   int iLines;
   char hStr[2];
   void Program (AnsiString Jmeno_Souboru, int System, int Port);
   unsigned char TForm1::GetHex(unsigned char* cPtr);
   bool TForm1::Communication(unsigned char Instr, unsigned char* VysBuff);
public:		// User declarations
   __fastcall TForm1(TComponent* Owner);
   bool TForm1::OpenCom();
   bool TForm1::ChangeComSpeed(int iSpeed);
   void TForm1::ConsoleChar(unsigned char chRead);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------

class TReadBase : public TThread
{
private:
protected:
   void __fastcall Execute();
public:
   bool bRunning;
   __fastcall TReadBase(bool CreateSuspended);
};
#endif
