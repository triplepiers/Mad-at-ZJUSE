VERSION 5.00
Begin VB.Form Main_User 
   Caption         =   "个人主页"
   ClientHeight    =   4485
   ClientLeft      =   6945
   ClientTop       =   5595
   ClientWidth     =   9330
   LinkTopic       =   "Form1"
   ScaleHeight     =   4485
   ScaleWidth      =   9330
   Begin VB.ListBox List1 
      Height          =   1680
      Index           =   2
      Left            =   4440
      TabIndex        =   14
      Top             =   1560
      Width           =   1815
   End
   Begin VB.CommandButton C_Query 
      Caption         =   "查询"
      Height          =   495
      Left            =   7440
      TabIndex        =   13
      Top             =   2880
      Width           =   1575
   End
   Begin VB.ListBox List1 
      Height          =   1680
      Index           =   1
      Left            =   2640
      TabIndex        =   11
      Top             =   1560
      Width           =   1815
   End
   Begin VB.ListBox List1 
      Height          =   1680
      Index           =   0
      Left            =   840
      TabIndex        =   9
      Top             =   1560
      Width           =   1815
   End
   Begin VB.CommandButton C_Logout 
      Caption         =   "登出"
      Height          =   495
      Left            =   7440
      TabIndex        =   4
      Top             =   3720
      Width           =   1575
   End
   Begin VB.Label Label8 
      Caption         =   "应还日期"
      Height          =   255
      Left            =   5040
      TabIndex        =   15
      Top             =   1320
      Width           =   855
   End
   Begin VB.Label Label7 
      Caption         =   "作者"
      Height          =   255
      Left            =   3360
      TabIndex        =   12
      Top             =   1320
      Width           =   615
   End
   Begin VB.Label Label6 
      Caption         =   "书名"
      Height          =   255
      Left            =   1560
      TabIndex        =   10
      Top             =   1320
      Width           =   615
   End
   Begin VB.Label cardid 
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   14.25
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   6840
      TabIndex        =   8
      Top             =   240
      Width           =   1575
   End
   Begin VB.Label Label5 
      Caption         =   "ID："
      BeginProperty Font 
         Name            =   "黑体"
         Size            =   14.25
         Charset         =   134
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   6000
      TabIndex        =   7
      Top             =   240
      Width           =   495
   End
   Begin VB.Label n_book 
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   14.25
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   2040
      TabIndex        =   6
      Top             =   720
      Width           =   1215
   End
   Begin VB.Label Label4 
      Caption         =   "已借阅数量："
      BeginProperty Font 
         Name            =   "黑体"
         Size            =   14.25
         Charset         =   134
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   240
      TabIndex        =   5
      Top             =   720
      Width           =   2055
   End
   Begin VB.Label Label3 
      Caption         =   "普通用户"
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   14.25
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   4440
      TabIndex        =   3
      Top             =   240
      Width           =   1935
   End
   Begin VB.Label Label2 
      Caption         =   "你的权限为："
      BeginProperty Font 
         Name            =   "黑体"
         Size            =   14.25
         Charset         =   134
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   2520
      TabIndex        =   2
      Top             =   240
      Width           =   2055
   End
   Begin VB.Label Label1 
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   14.25
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   1200
      TabIndex        =   1
      Top             =   240
      Width           =   1215
   End
   Begin VB.Label hello 
      Caption         =   "你好，"
      BeginProperty Font 
         Name            =   "黑体"
         Size            =   14.25
         Charset         =   134
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   240
      TabIndex        =   0
      Top             =   240
      Width           =   975
   End
End
Attribute VB_Name = "Main_User"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Public uid As String

Private Sub C_Logout_Click() '登出按钮
    Login.user_type = 0
     '关闭所有子页面
    F_Query.Hide

    Dim logout_status, tmp
    logout_status = MsgBox("要返回登录界面吗？", vbYesNo, "图书管理系统")
    
    If logout_status = vbYes Then
        Unload Main_User
        Login.Show
        Login.user_type = 0
    Else
        tmp = MsgBox("感谢使用, 已成功登出！", vbInformation, "图书管理系统")
        Unload Main_User
        End
    End If
End Sub

Private Sub c_query_Click() '前往图书查询界面
    F_Query.Show
End Sub

Private Sub Form_Load()
    Label1.Caption = Login.u_name '在主页显示用户名
    '获取读者卡号
    Dim conn As New ADODB.Connection
    conn.ConnectionString = "Driver={MySQL ODBC 8.0 Unicode Driver};Data Source=VBmySQL;Server=localhost;Database=booksys;User=root;Password=662258;Option=3"
    conn.Open '建立连接
    
    Dim rs As New ADODB.Recordset
    Set rs.ActiveConnection = conn
    rs.CursorLocation = 3
    
    '验证用户信息
    rs.Open "SELECT card FROM user WHERE name='" & Login.u_name & "'"
    uid = rs.Fields("card")
    cardid.Caption = uid
    rs.Close
    
    Dim rs1 As New ADODB.Recordset
    Set rs1.ActiveConnection = conn
    rs1.CursorLocation = 3
    rs1.Open "SELECT * FROM borrow WHERE c_no='" & uid & "'"
    '统计借阅数量
    Dim ct_book As Integer
    ct_book = rs1.RecordCount
    n_book.Caption = Str(ct_book)
    
    List1(0).Clear
    List1(1).Clear
    List1(2).Clear
    
    Dim cur_book As String
    Dim sql_book_info As ADODB.Recordset
    
    
    '输出书本信息
    Dim i As Integer
    For i = 1 To ct_book Step 1
        cur_book = rs1.Fields("b_no")
        
        Set sql_book_info = New ADODB.Recordset
        Set sql_book_info.ActiveConnection = conn
        sql_book_info.Open "SELECT * FROM book WHERE b_No='" & cur_book & "'"
        
        List1(0).AddItem sql_book_info.Fields("bookname")
        List1(1).AddItem sql_book_info.Fields("author")
        List1(2).AddItem rs1.Fields("returntime")
        sql_book_info.Close
        
        rs1.MoveNext
    Next
    
    rs1.Close
    conn.Close
End Sub

