VERSION 5.00
Begin VB.Form F_Query 
   Caption         =   "查询"
   ClientHeight    =   4380
   ClientLeft      =   6885
   ClientTop       =   5580
   ClientWidth     =   9210
   LinkTopic       =   "Form1"
   ScaleHeight     =   4380
   ScaleWidth      =   9210
   Begin VB.CommandButton c_query 
      Caption         =   "查询"
      Height          =   375
      Left            =   7200
      TabIndex        =   18
      Top             =   2520
      Width           =   1695
   End
   Begin VB.TextBox T_author 
      Height          =   375
      Left            =   7200
      TabIndex        =   17
      Top             =   1800
      Width           =   1695
   End
   Begin VB.TextBox T_bname 
      Height          =   375
      Left            =   7200
      TabIndex        =   15
      Top             =   1080
      Width           =   1695
   End
   Begin VB.ListBox List1 
      Height          =   2400
      Index           =   5
      Left            =   6120
      TabIndex        =   11
      Top             =   1080
      Width           =   855
   End
   Begin VB.ListBox List1 
      Height          =   2400
      Index           =   4
      Left            =   5280
      TabIndex        =   9
      Top             =   1080
      Width           =   855
   End
   Begin VB.ListBox List1 
      Height          =   2400
      Index           =   3
      Left            =   4440
      TabIndex        =   7
      Top             =   1080
      Width           =   855
   End
   Begin VB.ListBox List1 
      Height          =   2400
      Index           =   2
      Left            =   3600
      TabIndex        =   5
      Top             =   1080
      Width           =   855
   End
   Begin VB.ListBox List1 
      Height          =   2400
      Index           =   1
      Left            =   1920
      TabIndex        =   3
      Top             =   1080
      Width           =   1695
   End
   Begin VB.ListBox List1 
      Height          =   2400
      Index           =   0
      Left            =   240
      TabIndex        =   2
      Top             =   1080
      Width           =   1695
   End
   Begin VB.CommandButton C_BackMain 
      Caption         =   "返回"
      Height          =   495
      Left            =   240
      TabIndex        =   0
      Top             =   120
      Width           =   975
   End
   Begin VB.Label L_success 
      Caption         =   "查询成功"
      ForeColor       =   &H0000C000&
      Height          =   255
      Left            =   7680
      TabIndex        =   20
      Top             =   3000
      Width           =   855
   End
   Begin VB.Label L_blank 
      Caption         =   "至少有一项不为空"
      ForeColor       =   &H000000FF&
      Height          =   255
      Left            =   7320
      TabIndex        =   19
      Top             =   2280
      Width           =   1695
   End
   Begin VB.Label Label8 
      Caption         =   "待查作者："
      Height          =   255
      Left            =   7200
      TabIndex        =   16
      Top             =   1560
      Width           =   975
   End
   Begin VB.Label Label7 
      Caption         =   "待查书名："
      Height          =   255
      Left            =   7200
      TabIndex        =   14
      Top             =   840
      Width           =   975
   End
   Begin VB.Label Label6 
      Caption         =   "查询结果"
      BeginProperty Font 
         Name            =   "黑体"
         Size            =   14.25
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   3240
      TabIndex        =   13
      Top             =   240
      Width           =   1455
   End
   Begin VB.Label Label5 
      Caption         =   "库存"
      Height          =   255
      Left            =   6360
      TabIndex        =   12
      Top             =   840
      Width           =   495
   End
   Begin VB.Label Label4 
      Caption         =   "价格"
      Height          =   255
      Left            =   5520
      TabIndex        =   10
      Top             =   840
      Width           =   495
   End
   Begin VB.Label Label3 
      Caption         =   "出版年"
      Height          =   255
      Left            =   4560
      TabIndex        =   8
      Top             =   840
      Width           =   735
   End
   Begin VB.Label Label2 
      Caption         =   "出版社"
      Height          =   255
      Left            =   3720
      TabIndex        =   6
      Top             =   840
      Width           =   735
   End
   Begin VB.Label Label1 
      Caption         =   "作者"
      Height          =   255
      Left            =   2520
      TabIndex        =   4
      Top             =   840
      Width           =   495
   End
   Begin VB.Label l_b_name 
      Caption         =   "书名"
      Height          =   255
      Left            =   840
      TabIndex        =   1
      Top             =   840
      Width           =   495
   End
End
Attribute VB_Name = "F_Query"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub C_BackMain_Click() '返回主界面
    Call inilabel
    Call formclear
    T_bname.Text = ""
    T_author.Text = ""

    F_Query.Hide
    If Login.user_type = 1 Then
        Main_Admin.Show
    Else
        Main_User.Show
    End If
End Sub

Private Sub c_query_Click()
    Call inilabel
    Call formclear
    
    '检测，至少有一个不为空
    If Len(T_bname.Text) = 0 And Len(T_author.Text) = 0 Then
        L_blank.Caption = "至少有一项不为空"
        L_blank.Visible = True
        Exit Sub
    End If
    
    Dim conn As New ADODB.Connection
    conn.ConnectionString = "Driver={MySQL ODBC 8.0 Unicode Driver};Data Source=VBmySQL;Server=localhost;Database=booksys;User=root;Password=662258;Option=3"
    conn.Open '建立连接
    
    Dim rs As New ADODB.Recordset
    Set rs.ActiveConnection = conn
    rs.CursorLocation = 3
    
    rs.Open "SELECT * FROM book WHERE bookname LIKE '%" & T_bname.Text & "%' AND author LIKE '%" & T_author.Text & "%'"
    Dim size As Integer
    size = rs.RecordCount
    
    '未找到符合结果
    If size = 0 Then
        L_blank.Caption = "未找到结果"
        L_blank.Visible = True
        conn.Close
        Exit Sub
    End If
    
    '找到结果，显示
    'L_success.Caption = Str(size)
    L_Success.Visible = True
    
    Dim i As Integer
    For i = 1 To size
        List1(0).AddItem rs.Fields("bookname")
        List1(1).AddItem rs.Fields("author")
        List1(2).AddItem rs.Fields("publisher")
        List1(3).AddItem rs.Fields("pub_year")
        List1(4).AddItem rs.Fields("price") & "￥"
        List1(5).AddItem rs.Fields("storage")
        rs.MoveNext
    Next
    
    conn.Close
End Sub

Function inilabel() As Integer '初始化标签显示
    L_blank.Visible = False
    L_Success.Visible = False
End Function

Private Sub Form_Load()
    Call inilabel
End Sub

Function formclear() As Integer '清空所有list
    Dim i As Integer
    For i = 0 To 5 Step 1
        List1(i).Clear
    Next
End Function
