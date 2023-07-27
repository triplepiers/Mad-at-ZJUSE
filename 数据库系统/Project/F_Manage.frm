VERSION 5.00
Begin VB.Form F_Manage 
   Caption         =   "借书证管理"
   ClientHeight    =   4320
   ClientLeft      =   6885
   ClientTop       =   5745
   ClientWidth     =   9345
   LinkTopic       =   "Form1"
   ScaleHeight     =   4320
   ScaleWidth      =   9345
   Begin VB.CommandButton C_Del 
      Caption         =   "删除"
      Height          =   375
      Left            =   6600
      TabIndex        =   19
      Top             =   3240
      Width           =   2175
   End
   Begin VB.TextBox T_Del 
      Height          =   375
      Left            =   6600
      TabIndex        =   17
      Top             =   2520
      Width           =   2175
   End
   Begin VB.CommandButton printall 
      Caption         =   "显示所有"
      Height          =   495
      Left            =   120
      TabIndex        =   15
      Top             =   1320
      Width           =   975
   End
   Begin VB.CommandButton C_Search 
      Caption         =   "查询"
      Height          =   375
      Left            =   6600
      TabIndex        =   12
      Top             =   1440
      Width           =   2175
   End
   Begin VB.TextBox T_No 
      Height          =   375
      Left            =   6600
      TabIndex        =   11
      Top             =   720
      Width           =   2175
   End
   Begin VB.ListBox List1 
      Height          =   3300
      Index           =   3
      Left            =   5160
      TabIndex        =   5
      Top             =   720
      Width           =   1215
   End
   Begin VB.ListBox List1 
      Height          =   3300
      Index           =   2
      Left            =   3960
      TabIndex        =   4
      Top             =   720
      Width           =   1215
   End
   Begin VB.ListBox List1 
      Height          =   3300
      Index           =   1
      Left            =   2760
      TabIndex        =   3
      Top             =   720
      Width           =   1215
   End
   Begin VB.ListBox List1 
      Height          =   3300
      Index           =   0
      Left            =   1560
      TabIndex        =   2
      Top             =   720
      Width           =   1215
   End
   Begin VB.CommandButton C_New 
      Caption         =   "新建读者"
      Height          =   495
      Left            =   120
      TabIndex        =   1
      Top             =   720
      Width           =   975
   End
   Begin VB.CommandButton C_BackMain 
      Caption         =   "返回"
      Height          =   495
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   975
   End
   Begin VB.Label Label6 
      Caption         =   "删除成功"
      ForeColor       =   &H0000C000&
      Height          =   255
      Left            =   7320
      TabIndex        =   20
      Top             =   3720
      Width           =   855
   End
   Begin VB.Label Label5 
      Caption         =   "编号不可为空"
      ForeColor       =   &H000000FF&
      Height          =   255
      Left            =   7080
      TabIndex        =   18
      Top             =   3000
      Width           =   1215
   End
   Begin VB.Label Label4 
      Caption         =   "待删除编号:"
      Height          =   255
      Left            =   6600
      TabIndex        =   16
      Top             =   2280
      Width           =   1215
   End
   Begin VB.Label Label3 
      Caption         =   "查询成功"
      ForeColor       =   &H0000C000&
      Height          =   255
      Left            =   7320
      TabIndex        =   14
      Top             =   1920
      Width           =   855
   End
   Begin VB.Label Label2 
      Caption         =   "编号不可为空"
      ForeColor       =   &H000000FF&
      Height          =   255
      Left            =   7080
      TabIndex        =   13
      Top             =   1200
      Width           =   1215
   End
   Begin VB.Label Label1 
      Caption         =   "待查询编号:"
      Height          =   255
      Left            =   6600
      TabIndex        =   10
      Top             =   480
      Width           =   1215
   End
   Begin VB.Label L_Time 
      Caption         =   "过期时间"
      Height          =   255
      Left            =   5160
      TabIndex        =   9
      Top             =   480
      Width           =   735
   End
   Begin VB.Label L_Tel 
      Caption         =   "联系方式"
      Height          =   255
      Left            =   3960
      TabIndex        =   8
      Top             =   480
      Width           =   855
   End
   Begin VB.Label L_Name 
      Caption         =   "读者姓名"
      Height          =   255
      Left            =   2760
      TabIndex        =   7
      Top             =   480
      Width           =   855
   End
   Begin VB.Label L_No 
      Caption         =   "读者编号(升)"
      Height          =   255
      Left            =   1560
      TabIndex        =   6
      Top             =   480
      Width           =   1095
   End
End
Attribute VB_Name = "F_Manage"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Public ordermode As Integer

Private Sub C_BackMain_Click() '返回主界面
    F_Manage.Hide
    neo_reader.Hide
    If Login.user_type = 1 Then
        Main_Admin.Show
    Else
        Main_User.Show
    End If
End Sub

Private Sub C_Del_Click()
    Label5.Visible = False
    Label6.Visible = False
    
    Dim len_no, len_na, len_tel, len_ti As Integer
    len_no = Len(T_Del.Text)
    If len_no = 0 Then
        Label5.Caption = "编号不可为空"
        Label5.Visible = True
        Exit Sub
    End If
    
    Dim conn As New ADODB.Connection
    conn.ConnectionString = "Driver={MySQL ODBC 8.0 Unicode Driver};Data Source=VBmySQL;Server=localhost;Database=booksys;User=root;Password=662258;Option=3"
    conn.Open '建立连接
    
    Dim rs As New ADODB.Recordset
    Set rs.ActiveConnection = conn
    rs.CursorLocation = 3
    
    rs.Open "SELECT * FROM card WHERE c_no='" & T_Del.Text & "'"
    Dim size As Integer
    size = rs.RecordCount
    
    If size = 0 Then
        Label5.Caption = "该编号不存在"
        Label5.Visible = True
        rs.Close
        conn.Close
        Exit Sub
    End If
    rs.Close
    
    Dim rs1 As New ADODB.Recordset
    Set rs1.ActiveConnection = conn
    rs1.CursorLocation = 3
    rs1.Open "DELETE FROM card WHERE c_no='" & T_Del.Text & "'"
    conn.Close
    Label6.Visible = True
    Call ref
End Sub

Private Sub C_New_Click() '打开新建读者窗口
    neo_reader.Show
End Sub

Private Sub C_Search_Click()

    Label2.Visible = False
    Label3.Visible = False
    
    Dim len_no, len_na, len_tel, len_ti As Integer
    len_no = Len(T_No.Text)
    If len_no = 0 Then
        Label2.Caption = "编号不可为空"
        Label2.Visible = True
        Exit Sub
    End If
    
    List1(0).Clear
    List1(1).Clear
    List1(2).Clear
    List1(3).Clear
    
    Dim conn As New ADODB.Connection
    conn.ConnectionString = "Driver={MySQL ODBC 8.0 Unicode Driver};Data Source=VBmySQL;Server=localhost;Database=booksys;User=root;Password=662258;Option=3"
    conn.Open '建立连接
    
    Dim rs As New ADODB.Recordset
    Set rs.ActiveConnection = conn
    rs.CursorLocation = 3
    
    rs.Open "SELECT * FROM card WHERE c_no='" & T_No.Text & "'"
    Dim size As Integer
    size = rs.RecordCount
    
    If size = 0 Then
        Label2.Caption = "该编号不存在"
        Label2.Visible = True
        rs.Close
        conn.Close
        Exit Sub
    End If
    
    List1(0).AddItem rs.Fields("c_no")
    List1(1).AddItem rs.Fields("reader_name")
    List1(2).AddItem rs.Fields("tel")
    List1(3).AddItem rs.Fields("expiration_time")
    
    Label3.Visible = True
    rs.Close
    conn.Close
End Sub

Private Sub Form_Load()
    Call hidelabel
    Call ref
End Sub


Function hidelabel() As Integer
    Label2.Visible = False
    Label3.Visible = False
    Label5.Visible = False
    Label6.Visible = False
    hidelabel = 1
End Function
Function ref() As Integer
    '清空list
    List1(0).Clear
    List1(1).Clear
    List1(2).Clear
    List1(3).Clear
    
    Dim conn As New ADODB.Connection
    conn.ConnectionString = "Driver={MySQL ODBC 8.0 Unicode Driver};Data Source=VBmySQL;Server=localhost;Database=booksys;User=root;Password=662258;Option=3"
    conn.Open '建立连接
    
    Dim rs As New ADODB.Recordset
    Set rs.ActiveConnection = conn
    rs.CursorLocation = 3
    
    rs.Open "SELECT * FROM card ORDER BY c_no"
    Dim size As Integer
    size = rs.RecordCount
    
    For i = 1 To size Step 1
        List1(0).AddItem rs.Fields("c_no")
        List1(1).AddItem rs.Fields("reader_name")
        List1(2).AddItem rs.Fields("tel")
        List1(3).AddItem rs.Fields("expiration_time")
        rs.MoveNext
    Next
    ref = 1
End Function

Private Sub printall_Click()
    Call hidelabel
    Call ref
End Sub
