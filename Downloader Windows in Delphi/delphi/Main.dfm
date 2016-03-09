object MainForm: TMainForm
  Left = 258
  Top = 170
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'PIC downloader'
  ClientHeight = 254
  ClientWidth = 278
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010001002020100000000000E80200001600000028000000200000004000
    0000010004000000000080020000000000000000000000000000000000000000
    000000008000008000000080800080000000800080008080000080808000C0C0
    C0000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000000
    0000000000000000000000000000080000000000000000000000000000000F80
    00000000000000000000000000000FFF88888888888888888888888880000FFF
    88888888888888888888888880000FFF88888888888888888888888880000FFF
    88888888888888888888888880000FFF88888000000080000000888880000FFF
    888000FFFFF000FFFFF0008880000FFF888000FFFFFFFFFFFFF0008880000FFF
    888880FFFFFFFFFFFFF0888880000FFF888000FFFFFFFFFFFFF0008880000FFF
    888000FFFFFFFFFFFFF0008880000FFF888880FFFFFFFFFFFFF0888880000FFF
    888000FFFFFFFFFFFFF0008880000FFF888000FFFFFFFFFFFFF0008880000FFF
    888880FFFFFFFFFFFFF0888880000FFF888000FFFFFFFFFFFFF0008880000FFF
    888000FFFFFFFFFFFFF0008880000FFF888880FFFFFFFFFFFFF0888880000FFF
    888000FFFFFFFFFFFFF0008880000FFF888000FFFFFFFFFFFFF0008880000FFF
    888880FFFFFFFFFFFFF0888880000FFF888000FFFFFFFFFFFFF0008880000FFF
    888000FFFFFFFFFFFFF0008880000FFF88888000000000000000888880000FFF
    88888888888888888888888880000FFF88888888888888888888888880000FFF
    FFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF8000FFF
    FFFFFFFFFFFFFFFFFFFFFFFFFF80000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000000000000000000000000000}
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 8
    Top = 20
    Width = 16
    Height = 13
    Caption = 'File'
  end
  object Label3: TLabel
    Left = 8
    Top = 60
    Width = 19
    Height = 13
    Caption = 'Port'
  end
  object Label4: TLabel
    Left = 8
    Top = 128
    Width = 18
    Height = 13
    Caption = 'Info'
  end
  object Bevel1: TBevel
    Left = 0
    Top = 96
    Width = 281
    Height = 17
    Shape = bsTopLine
  end
  object Bevel2: TBevel
    Left = 0
    Top = 208
    Width = 281
    Height = 10
    Shape = bsBottomLine
  end
  object Label1: TLabel
    Left = 40
    Top = 224
    Width = 197
    Height = 13
    Caption = #169' 2000 EHL elektronika, Petr Kolomaznik'
  end
  object Label5: TLabel
    Left = 160
    Top = 60
    Width = 13
    Height = 13
    Caption = 'Bd'
  end
  object Label6: TLabel
    Left = 8
    Top = 240
    Width = 105
    Height = 13
    Caption = 'http://www.ehl.cz/pic'
  end
  object Label7: TLabel
    Left = 200
    Top = 240
    Width = 61
    Height = 13
    Caption = 'FREEWARE'
  end
  object Button1: TButton
    Left = 200
    Top = 14
    Width = 65
    Height = 25
    Caption = 'Search (F2)'
    TabOrder = 1
    OnClick = Button1Click
  end
  object Edit1: TEdit
    Left = 32
    Top = 16
    Width = 153
    Height = 21
    Enabled = False
    TabOrder = 0
  end
  object ComboBox2: TComboBox
    Left = 32
    Top = 56
    Width = 57
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 2
    OnChange = ComboBox2Change
    Items.Strings = (
      'COM1'
      'COM2'
      'COM3'
      'COM4'
      'COM5'
      'COM6')
  end
  object Button2: TButton
    Left = 56
    Top = 176
    Width = 75
    Height = 25
    Caption = 'Write (F4)'
    TabOrder = 5
    OnClick = Button2Click
  end
  object Edit2: TEdit
    Left = 32
    Top = 112
    Width = 225
    Height = 21
    TabStop = False
    Enabled = False
    TabOrder = 6
  end
  object ProgressBar1: TProgressBar
    Left = 32
    Top = 144
    Width = 225
    Height = 16
    Smooth = True
    TabOrder = 7
  end
  object ComboBox1: TComboBox
    Left = 96
    Top = 56
    Width = 57
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 3
    OnChange = ComboBox1Change
    Items.Strings = (
      '2400'
      '4800'
      '9600'
      '19200'
      '38400'
      '56000')
  end
  object CheckBox1: TCheckBox
    Left = 196
    Top = 58
    Width = 69
    Height = 17
    Caption = 'EEPROM'
    TabOrder = 4
    OnClick = CheckBox1Click
  end
  object Button3: TButton
    Left = 160
    Top = 176
    Width = 73
    Height = 25
    Caption = 'Cancel (ESC)'
    TabOrder = 8
    OnClick = Button3Click
  end
  object OpenDialog1: TOpenDialog
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Top = 152
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 250
    OnTimer = Timer1Timer
    Top = 184
  end
end
