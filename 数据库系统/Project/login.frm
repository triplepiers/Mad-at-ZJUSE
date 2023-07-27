VERSION 5.00
Begin VB.Form Login 
   Caption         =   "图书管理系统"
   ClientHeight    =   3015
   ClientLeft      =   9315
   ClientTop       =   6300
   ClientWidth     =   4560
   LinkTopic       =   "Form1"
   ScaleHeight     =   3015
   ScaleWidth      =   4560
   Begin VB.CommandButton Create 
      Caption         =   "注册"
      Height          =   495
      Left            =   3240
      TabIndex        =   8
      Top             =   2400
      Width           =   1215
   End
   Begin VB.Timer Timer1 
      Enabled         =   0   'False
      Interval        =   500
      Left            =   240
      Top             =   240
   End
   Begin VB.CommandButton C_Login 
      Caption         =   "点击登录"
      Height          =   495
      Left            =   1560
      Style           =   1  'Graphical
      TabIndex        =   5
      Top             =   2400
      Width           =   1455
   End
   Begin VB.TextBox I_Upass 
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   10.5
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      IMEMode         =   3  'DISABLE
      Left            =   1560
      PasswordChar    =   "*"
      TabIndex        =   4
      Text            =   "123456"
      Top             =   1560
      Width           =   1455
   End
   Begin VB.TextBox I_Uname 
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   10.5
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   1560
      TabIndex        =   3
      Text            =   "admin"
      Top             =   720
      Width           =   1455
   End
   Begin VB.Label W_Upass 
      Caption         =   "密码不可为空！"
      ForeColor       =   &H000000FF&
      Height          =   375
      Left            =   1560
      TabIndex        =   7
      Top             =   2160
      Visible         =   0   'False
      Width           =   1575
   End
   Begin VB.Label W_Uname 
      Caption         =   "用户名不可为空！"
      ForeColor       =   &H000000FF&
      Height          =   375
      Left            =   1560
      TabIndex        =   6
      Top             =   1320
      Visible         =   0   'False
      Width           =   1575
   End
   Begin VB.Label L_Upass 
      Caption         =   "密 码："
      BeginProperty Font 
         Name            =   "黑体"
         Size            =   10.5
         Charset         =   134
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   600
      TabIndex        =   2
      Top             =   1680
      Width           =   855
   End
   Begin VB.Label L_Welcom 
      Caption         =   "登   录"
      BeginProperty Font 
         Name            =   "黑体"
         Size            =   15.75
         Charset         =   134
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   615
      Left            =   1560
      TabIndex        =   1
      Top             =   120
      Width           =   1455
   End
   Begin VB.Label L_Uname 
      Caption         =   "用户名："
      BeginProperty Font 
         Name            =   "黑体"
         Size            =   10.5
         Charset         =   134
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   480
      TabIndex        =   0
      Top             =   840
      Width           =   975
   End
End
Attribute VB_Name = "Login"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Public user_type As Integer '用户类型：0-stu；1-admin
Public u_name As String '用户名

Private Sub Create_Click() '跳转至用户界面
    Login.Hide
    F_SignIn.Show
    Call inilabel
    Call textclear
End Sub

'美招看到一定大为欣慰吧，VB居然有文艺复兴的一天
Private Sub Form_Load()
    user_type = 0 '重置用户状态
End Sub

Private Sub C_Login_Click() '单击登录按钮
    Call inilabel
    
    Dim u_pass As String '密码
    
    u_name = I_Uname.Text
    u_pass = I_Upass.Text
    
    Dim len_u_name As Integer
    len_u_name = Len(u_name)
    
    '用户名为空
    If (len_u_name = 0) Then
        W_Uname.Caption = "用户名不可为空！"
        W_Uname.Visible = True
    End If
    
    Dim len_u_pass As Integer
    len_u_pass = Len(u_pass)
    
    '密码为空
    If (len_u_pass = 0) Then
        W_Upass.Caption = "密码不可为空！"
        W_Upass.Visible = True
    End If
    
    'name/password中一项为空则直接返回
    If (len_u_name = 0 Or len_u_pass = 0) Then
       Exit Sub
    End If
    Dim conn As New ADODB.Connection
    conn.ConnectionString = "Driver={MySQL ODBC 8.0 Unicode Driver};Data Source=VBmySQL;Server=localhost;Database=booksys;User=root;Password=662258;Option=3"
    conn.Open '建立连接
    
    Dim rs As New ADODB.Recordset
    Set rs.ActiveConnection = conn
    rs.CursorLocation = 3
    
    '验证用户信息
    rs.Open "SELECT * FROM user WHERE name='" & u_name & "' AND password='" & u_pass & "'"
    Dim size As Integer
    size = rs.RecordCount
    
    
    If size = 0 Then
        W_Upass.Caption = "账号或密码错误！"
        W_Upass.Visible = True
    Else
        '登录有效，区分用户类型 admin / user
        user_type = rs.Fields("type")
        'Label3.Caption = "usertype=" & Str(user_type)
        C_Login.Caption = "登录成功"
        Timer1.Enabled = True
    End If
    
    rs.Close
    conn.Close '断开连接
End Sub

Private Sub Timer1_Timer()
    Call inilabel
    Call textclear
    Login.Hide
    If user_type = 1 Then
        Main_Admin.Show
    Else
        Main_User.Show
    End If
    Timer1.Enabled = False
    C_Login.Caption = "点击登录"
End Sub

Function inilabel() As Integer
    '将错误提示初始化为不可见
    W_Uname.Visible = False
    W_Upass.Visible = False
End Function

Function textclear() As Integer
    '清空文本框
    I_Uname.Text = ""
    I_Upass.Text = ""
End Function
