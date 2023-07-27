VERSION 5.00
Begin VB.Form neo_reader 
   Caption         =   "新建读者"
   ClientHeight    =   4365
   ClientLeft      =   3285
   ClientTop       =   5685
   ClientWidth     =   3570
   LinkTopic       =   "Form1"
   ScaleHeight     =   4365
   ScaleWidth      =   3570
   Begin VB.CommandButton Create_Reader 
      Caption         =   "确认添加"
      Height          =   495
      Left            =   1440
      TabIndex        =   12
      Top             =   3600
      Width           =   1575
   End
   Begin VB.TextBox T_Tel 
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   1440
      TabIndex        =   10
      Text            =   "18357791811"
      Top             =   2880
      Width           =   1575
   End
   Begin VB.TextBox T_Time 
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   1440
      TabIndex        =   7
      Text            =   "20070754"
      Top             =   2040
      Width           =   1575
   End
   Begin VB.TextBox T_Name 
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   1440
      TabIndex        =   4
      Text            =   "好耶"
      Top             =   1200
      Width           =   1575
   End
   Begin VB.TextBox T_No 
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   360
      Left            =   1440
      TabIndex        =   1
      Text            =   "3200104323"
      Top             =   360
      Width           =   1575
   End
   Begin VB.Label Label4 
      Caption         =   "TIP:日期请以 yyyy/mm/dd 形式输入"
      Height          =   255
      Left            =   360
      TabIndex        =   14
      Top             =   120
      Width           =   2895
   End
   Begin VB.Label L_Success 
      Caption         =   "添加成功"
      ForeColor       =   &H0000C000&
      Height          =   255
      Left            =   360
      TabIndex        =   13
      Top             =   3720
      Visible         =   0   'False
      Width           =   975
   End
   Begin VB.Label W_Tel 
      Caption         =   "联系方式不可为空"
      ForeColor       =   &H000000FF&
      Height          =   255
      Left            =   1560
      TabIndex        =   11
      Top             =   3360
      Visible         =   0   'False
      Width           =   1575
   End
   Begin VB.Label Label6 
      Caption         =   "联系方式："
      BeginProperty Font 
         Name            =   "黑体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   240
      TabIndex        =   9
      Top             =   3000
      Width           =   1215
   End
   Begin VB.Label W_T 
      Caption         =   "有效期不可为空"
      ForeColor       =   &H000000FF&
      Height          =   255
      Left            =   1680
      TabIndex        =   8
      Top             =   2520
      Visible         =   0   'False
      Width           =   1335
   End
   Begin VB.Label Label3 
      Caption         =   "过期时间："
      BeginProperty Font 
         Name            =   "黑体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   240
      TabIndex        =   6
      Top             =   2160
      Width           =   1575
   End
   Begin VB.Label W_N 
      Caption         =   "姓名不可为空"
      ForeColor       =   &H000000FF&
      Height          =   255
      Left            =   1680
      TabIndex        =   5
      Top             =   1680
      Visible         =   0   'False
      Width           =   1455
   End
   Begin VB.Label Label2 
      Caption         =   "读者姓名："
      BeginProperty Font 
         Name            =   "黑体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   240
      TabIndex        =   3
      Top             =   1320
      Width           =   1335
   End
   Begin VB.Label W_No 
      Caption         =   "编号不可为空"
      ForeColor       =   &H000000FF&
      Height          =   255
      Left            =   1680
      TabIndex        =   2
      Top             =   840
      Visible         =   0   'False
      Width           =   1095
   End
   Begin VB.Label Label1 
      Caption         =   "读者编号："
      BeginProperty Font 
         Name            =   "黑体"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   240
      TabIndex        =   0
      Top             =   480
      Width           =   1215
   End
End
Attribute VB_Name = "neo_reader"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Create_Reader_Click() '新建读者
    '初始化错误提示
    W_No.Visible = False
    W_N.Visible = False
    W_T.Visible = False
    W_Tel.Visible = False
    L_Success.Visible = False
    
    Dim c_no, name, time, tel As String
    Dim l_c_no, L_Name, L_Time, L_Tel As Integer
    l_c_no = Len(T_No.Text)
    L_Name = Len(T_Name.Text)
    L_Time = Len(T_Time.Text)
    L_Tel = Len(T_Tel.Text)
    
    Dim isError As Integer
    isError = 0
    
    If l_c_no = 0 Then
        W_No.Caption = "编号不可为空"
        W_No.Visible = True
        isError = 1
    End If
    
    If L_Name = 0 Then
        W_N.Caption = "姓名不可为空"
        W_N.Visible = True
        isError = 1
    End If
    
    If L_Time = 0 Then
        W_T.Caption = "有效期不可为空"
        W_T.Visible = True
        isError = 1
    End If
    
    If L_Tel = 0 Then
        W_Tel.Caption = "联系方式不可为空"
        W_Tel.Visible = True
        isError = 1
    End If
    
    '判断电话号码的有效性
    If L_Tel <> 11 Then
        W_Tel.Caption = "无效联系方式"
        W_Tel.Visible = True
        Exit Sub
    ElseIf Not IsNumeric(T_Tel.Text) Then '出现非数字字符
        W_Tel.Caption = "非法联系方式"
        W_Tel.Visible = True
        isError = 1
    End If
    
    If Not IsDate(T_Time.Text) Then
        W_T.Caption = "非法日期"
        W_T.Visible = True
        isError = 1
    End If
    
    If isError = 0 Then
        '验证卡号可用性
        Dim sql_check_no As String
        sql_check_no = "SELECT c_no FROM card WHERE c_no='" & T_No.Text & "'"
        Dim conn As New ADODB.Connection
        conn.ConnectionString = "Driver={MySQL ODBC 8.0 Unicode Driver};Data Source=VBmySQL;Server=localhost;Database=booksys;User=root;Password=662258;Option=3"
        conn.Open '建立连接
        
        Dim rs As New ADODB.Recordset
        Set rs.ActiveConnection = conn
        rs.CursorLocation = 3
        rs.Open sql_check_no
        Dim size As Integer
        size = rs.RecordCount
        If size > 0 Then
            
            W_No.Caption = "卡号已占用"
            W_No.Visible = True
            rs.Close
            conn.Close
            Exit Sub
        End If
        
        rs.Close
        
        Dim rs1 As New ADODB.Recordset
        Set rs1.ActiveConnection = conn
        rs1.CursorLocation = 3
        
        '卡号合法，插入
        Dim sql As String
        sql = "INSERT INTO card VALUES ('" & T_No.Text & "','" & T_Name.Text & "','" & T_Tel.Text & "','" & T_Time.Text & "')"
        rs1.Open sql
        L_Success.Visible = True '提示插入成功
        conn.Close '关闭连接
        Call F_Manage.ref '刷新显示
    Else '存在非法输入
        Exit Sub
    End If
        
End Sub
