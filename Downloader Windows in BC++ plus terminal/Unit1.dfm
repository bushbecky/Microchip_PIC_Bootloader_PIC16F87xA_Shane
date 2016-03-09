object Form1: TForm1
  Left = 375
  Top = 153
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = ' PIC Bootloader+'
  ClientHeight = 496
  ClientWidth = 298
  Color = clSilver
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010001002020100000000000E80200001600000028000000200000004000
    0000010004000000000080020000000000000000000000000000000000000000
    0000000080000080000000808000800000008000800080800000C0C0C0008080
    80000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    00000000F00000F000000000000000000000000F0F000F0F0000000000000000
    000000F0F00000F0F00000000000000000000F0F0000000F0F00000000000000
    0000F0F000000000F0F0000000000000000F0F00000000000F0F000000000000
    00F0F0000000000000F0F000000000000F0F000000000000000F0F0000000000
    F0F00000000000000000F0F00000000F0F0000000000000000000F0000000000
    F000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000F0000000000F0000000000000000000F0F00000000F
    0F00000000000000000F0F0000000000F0F000000000000000F0F00000000000
    0F0F0000000000000F0F00000000000000F0F00000000000F0F0000000000000
    000F0F000000000F0F000000000000000000F0F0000000F0F000000000000000
    00000F0F00000F0F0000000000000000000000F0F000F0F00000000000000000
    0000000F00000F00000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000000000000000000000000000FFFF
    FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7DFFFFFEAAFFFFFD457FFFFA82BFFFF50
    15FFFEA00AFFFD40057FFA8002BFF500015FEA0000BFF400007FF800007FF800
    00BFF400015FEA0002BFF500057FFA800AFFFD4015FFFEA02BFFFF5057FFFFA8
    AFFFFFD55FFFFFEFBFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnKeyPress = FormKeyPress
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 8
    Top = 4
    Width = 16
    Height = 13
    Caption = 'File'
  end
  object Label3: TLabel
    Left = 8
    Top = 44
    Width = 19
    Height = 13
    Caption = 'Port'
  end
  object Label5: TLabel
    Left = 80
    Top = 44
    Width = 48
    Height = 13
    Caption = 'BaudRate'
  end
  object Label1: TLabel
    Left = 8
    Top = 84
    Width = 18
    Height = 13
    Caption = 'Info'
  end
  object Label4: TLabel
    Left = 216
    Top = 196
    Width = 48
    Height = 13
    Caption = 'BaudRate'
  end
  object Bevel1: TBevel
    Left = 8
    Top = 160
    Width = 281
    Height = 17
    Shape = bsBottomLine
  end
  object SpeedButton2: TSpeedButton
    Left = 240
    Top = 136
    Width = 49
    Height = 25
    Caption = 'Cancel '
    Font.Charset = ANSI_CHARSET
    Font.Color = clMaroon
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsItalic]
    ParentFont = False
    OnClick = SpeedButton2Click
  end
  object SpeedButton3: TSpeedButton
    Left = 184
    Top = 136
    Width = 49
    Height = 25
    Caption = 'Write'
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsItalic]
    ParentFont = False
    OnClick = SpeedButton3Click
  end
  object ProductName: TLabel
    Left = 8
    Top = 480
    Width = 278
    Height = 15
    Caption = 'Bootloader+  V1.02    by     Herman  Aartsen   TNO - NL'
    Color = clNone
    Font.Charset = ANSI_CHARSET
    Font.Color = clGray
    Font.Height = -11
    Font.Name = 'Comic Sans MS'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = True
    IsControl = True
  end
  object SpeedButton1: TSpeedButton
    Left = 216
    Top = 248
    Width = 65
    Height = 25
    Caption = 'Clear '
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsItalic]
    ParentFont = False
    OnClick = SpeedButton1Click
  end
  object SpeedButton4: TSpeedButton
    Left = 240
    Top = 16
    Width = 49
    Height = 25
    Caption = 'Search '
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsItalic]
    ParentFont = False
    OnClick = SpeedButton4Click
  end
  object ComboBox2: TComboBox
    Left = 8
    Top = 56
    Width = 57
    Height = 21
    TabStop = False
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
    OnChange = ComboBox2Change
    OnKeyDown = ComboBox1KeyDown
    Items.Strings = (
      'COM1'
      'COM2'
      'COM3'
      'COM4')
  end
  object Edit2: TEdit
    Left = 8
    Top = 96
    Width = 281
    Height = 21
    TabStop = False
    Enabled = False
    TabOrder = 1
  end
  object ProgressBar1: TProgressBar
    Left = 8
    Top = 136
    Width = 161
    Height = 24
    Min = 0
    Max = 100
    Smooth = True
    TabOrder = 2
  end
  object ComboBox1: TComboBox
    Left = 80
    Top = 56
    Width = 65
    Height = 21
    TabStop = False
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 3
    OnChange = ComboBox1Change
    OnKeyDown = ComboBox1KeyDown
    Items.Strings = (
      '2400'
      '4800'
      '9600'
      '19200'
      '38400'
      '56000')
  end
  object CheckBox1: TCheckBox
    Left = 164
    Top = 58
    Width = 69
    Height = 17
    TabStop = False
    Caption = 'EEPROM'
    TabOrder = 4
    OnKeyDown = ComboBox1KeyDown
  end
  object Memo1: TMemo
    Left = 8
    Top = 296
    Width = 281
    Height = 177
    Color = clWhite
    Font.Charset = OEM_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Terminal'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssHorizontal
    TabOrder = 5
    WordWrap = False
  end
  object ComboBox3: TComboBox
    Left = 216
    Top = 208
    Width = 65
    Height = 21
    TabStop = False
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 6
    OnChange = ComboBox3Change
    OnKeyDown = ComboBox1KeyDown
    Items.Strings = (
      '2400'
      '4800'
      '9600'
      '19200'
      '38400'
      '56000')
  end
  object CheckBox2: TCheckBox
    Left = 28
    Top = 202
    Width = 85
    Height = 17
    TabStop = False
    Caption = 'Local Echo'
    TabOrder = 7
    OnClick = CheckBox2Click
    OnKeyDown = ComboBox1KeyDown
  end
  object CheckBox3: TCheckBox
    Left = 28
    Top = 226
    Width = 93
    Height = 17
    TabStop = False
    Caption = 'CR->CR+LF in'
    TabOrder = 8
    OnClick = CheckBox3Click
    OnKeyDown = ComboBox1KeyDown
  end
  object CheckBox4: TCheckBox
    Left = 28
    Top = 250
    Width = 93
    Height = 17
    TabStop = False
    Caption = 'CR->CR+LF out'
    TabOrder = 9
    OnClick = CheckBox4Click
    OnKeyDown = ComboBox1KeyDown
  end
  object Edit1: TEdit
    Left = 8
    Top = 16
    Width = 217
    Height = 21
    TabStop = False
    Enabled = False
    TabOrder = 10
  end
  object OpenDialog1: TOpenDialog
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 128
    Top = 240
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 250
    OnTimer = Timer1Timer
    Left = 128
    Top = 208
  end
end
