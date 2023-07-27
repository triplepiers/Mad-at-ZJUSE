VERSION 5.00
Begin VB.Form F_BorR 
   Caption         =   "借还书"
   ClientHeight    =   4215
   ClientLeft      =   6885
   ClientTop       =   5745
   ClientWidth     =   9270
   LinkTopic       =   "Form1"
   ScaleHeight     =   4215
   ScaleWidth      =   9270
   Begin VB.CommandButton Command4 
      Caption         =   "还书"
      Height          =   495
      Left            =   240
      TabIndex        =   20
      Top             =   3480
      Width           =   975
   End
   Begin VB.ListBox List1 
      Height          =   2760
      Left            =   3240
      TabIndex        =   14
      Top             =   1320
      Width           =   3735
   End
   Begin VB.Frame Frame1 
      Caption         =   "借阅信息查询"
      Height          =   3495
      Left            =   7080
      TabIndex        =   6
      Top             =   720
      Width           =   2175
      Begin VB.CommandButton Command3 
         Caption         =   "查询图书借阅情况"
         Height          =   375
         Left            =   120
         TabIndex        =   13
         Top             =   2640
         Width           =   1935
      End
      Begin VB.CommandButton Command2 
         Caption         =   "查询读者借阅情况"
         Height          =   375
         Left            =   120
         TabIndex        =   11
         Top             =   1080
         Width           =   1935
      End
      Begin VB.TextBox out_book 
         Height          =   375
         Left            =   120
         TabIndex        =   10
         Text            =   "Text4"
         Top             =   2040
         Width           =   1935
      End
      Begin VB.TextBox out_reader 
         Height          =   375
         Left            =   120
         TabIndex        =   9
         Text            =   "3180103606"
         Top             =   480
         Width           =   1935
      End
      Begin VB.Label Label6 
         Caption         =   "输入不可为空"
         ForeColor       =   &H000000FF&
         Height          =   255
         Left            =   480
         TabIndex        =   16
         Top             =   2400
         Width           =   1455
      End
      Begin VB.Label Label5 
         Caption         =   "输入不可为空"
         ForeColor       =   &H000000FF&
         Height          =   255
         Left            =   480
         TabIndex        =   15
         Top             =   840
         Width           =   1335
      End
      Begin VB.Label Label4 
         Caption         =   "待查图书编号："
         Height          =   255
         Left            =   120
         TabIndex        =   8
         Top             =   1800
         Width           =   1455
      End
      Begin VB.Label Label3 
         Caption         =   "待查读者编号："
         Height          =   255
         Left            =   120
         TabIndex        =   7
         Top             =   240
         Width           =   1455
      End
   End
   Begin VB.CommandButton Command1 
      Caption         =   "借阅"
      Height          =   495
      Left            =   240
      TabIndex        =   5
      Top             =   2760
      Width           =   975
   End
   Begin VB.TextBox in_book 
      Height          =   375
      Left            =   240
      TabIndex        =   4
      Top             =   2040
      Width           =   1695
   End
   Begin VB.TextBox in_reader 
      Height          =   375
      Left            =   240
      TabIndex        =   1
      Top             =   1080
      Width           =   1695
   End
   Begin VB.CommandButton C_BackMain 
      Caption         =   "返回"
      Height          =   495
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   975
   End
   Begin VB.Label Label12 
      Caption         =   "可在右侧输入读者编号查询该读者借阅情况，或查阅某一书籍借阅情况"
      Height          =   255
      Left            =   1320
      TabIndex        =   23
      Top             =   600
      Width           =   6135
   End
   Begin VB.Label Label11 
      Caption         =   "在左侧输入读者编号及图书编号，点击相应的按钮以进行借书 / 还书操作"
      Height          =   255
      Left            =   1320
      TabIndex        =   22
      Top             =   360
      Width           =   6375
   End
   Begin VB.Label Label13 
      Caption         =   "TIP："
      Height          =   255
      Left            =   1320
      TabIndex        =   24
      Top             =   120
      Width           =   975
   End
   Begin VB.Label Label10 
      Caption         =   "还书成功"
      ForeColor       =   &H0000C000&
      Height          =   255
      Left            =   1440
      TabIndex        =   21
      Top             =   3600
      Width           =   735
   End
   Begin VB.Label Label9 
      Caption         =   "借阅成功"
      ForeColor       =   &H0000C000&
      Height          =   255
      Left            =   1440
      TabIndex        =   19
      Top             =   2880
      Width           =   735
   End
   Begin VB.Label Label8 
      Caption         =   "输入不可为空"
      ForeColor       =   &H000000FF&
      Height          =   255
      Left            =   240
      TabIndex        =   18
      Top             =   2400
      Width           =   1455
   End
   Begin VB.Label Label7 
      Caption         =   "输入不可为空"
      ForeColor       =   &H000000FF&
      Height          =   255
      Left            =   240
      TabIndex        =   17
      Top             =   1440
      Width           =   1455
   End
   Begin VB.Label l_output 
      Caption         =   "反馈结果："
      Height          =   255
      Left            =   3240
      TabIndex        =   12
      Top             =   960
      Width           =   3975
   End
   Begin VB.Label Label2 
      Caption         =   "图书编号："
      Height          =   255
      Left            =   240
      TabIndex        =   3
      Top             =   1800
      Width           =   975
   End
   Begin VB.Label Label1 
      Caption         =   "读者编号："
      Height          =   255
      Left            =   240
      TabIndex        =   2
      Top             =   840
      Width           =   1095
   End
End
Attribute VB_Name = "F_BorR"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub C_BackMain_Click() '返回主界面
    Call ini(0)
    F_BorR.Hide
    If Login.user_type = 1 Then
        Main_Admin.Show
    Else
        Main_User.Show
    End If
End Sub

Function ini(n_box As Integer) As Integer
    '清空文本框内容
    If (n_box <> 1) Then
        in_reader.Text = ""
        in_book.Text = ""
    End If
    If (n_box <> 2) Then out_reader.Text = ""
    If (n_box <> 3) Then out_book.Text = ""
    '初始化标签状态
    Label5.Visible = False
    Label6.Visible = False
    Label7.Visible = False
    Label8.Visible = False
    Label9.Visible = False
    Label10.Visible = False
    l_output.Caption = "反馈结果："
    List1.Clear
End Function

Private Sub Command1_Click() '插入单条借阅信息
    Call ini(1)
    Dim isErr As Integer
    isErr = 0
    
    If Len(in_reader.Text) = 0 Then
        Label7.Caption = "输入不可为空"
        Label7.Visible = True
        isErr = 1
    End If
    
    If Len(in_book.Text) = 0 Then
        Label8.Caption = "输入不可为空"
        Label8.Visible = True
        isErr = 1
    End If
    '有空输入则退出
    If isErr = 1 Then Exit Sub
    
    '检查借阅信息是否存在
    Dim ck As Integer
    ck = checkcase()
    If ck = 1 Then
        Label8.Caption = "借阅信息已存在"
        Label8.Visible = True
        Exit Sub
    End If
    
    Dim conn As New ADODB.Connection
    conn.ConnectionString = "Driver={MySQL ODBC 8.0 Unicode Driver};Data Source=VBmySQL;Server=localhost;Database=booksys;User=root;Password=662258;Option=3"
    conn.Open '建立连接
        
    '验证读者编号是否有效
    Dim check_reader As New ADODB.Recordset
    Set check_reader.ActiveConnection = conn
    check_reader.CursorLocation = 3
    
    check_reader.Open "SELECT * FROM card WHERE c_no='" & in_reader.Text & "'"
    If check_reader.RecordCount = 0 Then
        Label7.Caption = "读者编号不存在"
        Label7.Visible = True
        check_reader.Close
        conn.Close
        Exit Sub
    End If
    
    '验证图书编号是否有效
    Dim check_book As New ADODB.Recordset
    Set check_book.ActiveConnection = conn
    check_book.CursorLocation = 3
    
    check_book.Open "SELECT * FROM book WHERE b_No='" & in_book.Text & "'"
    If check_book.RecordCount = 0 Then
        Label8.Caption = "图书编号不存在"
        Label8.Visible = True
        check_book.Close
        conn.Close
        Exit Sub
    ElseIf check_book.Fields("storage") = "0" Then
        Label8.Caption = "图书库存不足"
        Label8.Visible = True
        check_book.Close
        conn.Close
        Exit Sub
    End If
    
    '输出成功提示
    Label9.Visible = True
    '向borrow插入借书记录
    Dim neo_borrow As New ADODB.Recordset
    Set neo_borrow.ActiveConnection = conn
    neo_borrow.CursorLocation = 3
    neo_borrow.Open "INSERT INTO borrow VALUES ('" & in_reader.Text & "','" & in_book.Text & "','" & DateAdd("d", 30, Date) & "')"
    
    'book中库存-1
    Dim alter_storage As New ADODB.Recordset
    Set alter_storage.ActiveConnection = conn
    alter_storage.CursorLocation = 3
    alter_storage.Open "UPDATE book SET storage=storage-1 WHERE b_No='" & in_book.Text & "'"
    conn.Close
    Call showborrow
End Sub

Private Sub Command2_Click() '读者借阅情况
    Call ini(2)
    If Len(out_reader.Text) = 0 Then
        Label5.Caption = "输入不可为空"
        Label5.Visible = True
        Exit Sub
    End If
    '检查编号是否有效
    Dim conn As New ADODB.Connection
    conn.ConnectionString = "Driver={MySQL ODBC 8.0 Unicode Driver};Data Source=VBmySQL;Server=localhost;Database=booksys;User=root;Password=662258;Option=3"
    conn.Open '建立连接
    
    Dim rs As New ADODB.Recordset
    Set rs.ActiveConnection = conn
    rs.CursorLocation = 3
    
    '验证用户信息
    rs.Open "SELECT * FROM card WHERE c_no='" & out_reader.Text & "'"
    If rs.RecordCount = 0 Then
        Label5.Caption = "读者编号不存在"
        Label5.Visible = True
        rs.Close
        conn.Close
        Exit Sub
    End If
    
    Dim rs1 As New ADODB.Recordset
    Set rs1.ActiveConnection = conn
    rs1.CursorLocation = 3
    
    rs1.Open "SELECT * FROM borrow WHERE c_no='" & out_reader.Text & "'"
    Dim size As Integer
    size = rs1.RecordCount
    
    l_output.Caption = "编号为" & out_reader.Text & "的读者共借阅了" & Str(size) & "本书"
    
    Dim cur_bookno As String '当前图书编号
    Dim sql_book_info As ADODB.Recordset
    '在list中显示该编号读者借阅的图书编号+书名
    For i = 1 To size Step 1
        cur_bookno = rs1.Fields("b_no")
                
        Set sql_book_info = New ADODB.Recordset
        Set sql_book_info.ActiveConnection = conn
        sql_book_info.Open "SELECT * FROM book WHERE b_No='" & cur_bookno & "'"
        List1.AddItem rs1.Fields("returntime") & "  " & sql_book_info.Fields("b_No") & "  " & sql_book_info.Fields("bookname")
        sql_book_info.Close
        
        rs1.MoveNext
    Next
End Sub

Private Sub Command3_Click() '给定图书编号，输出借阅数、余量、读者编号
    Call ini(3)
    If Len(out_book.Text) = 0 Then
        Label6.Caption = "输入不可为空"
        Label6.Visible = True
        Exit Sub
    End If
    
    '检查编号是否有效
    Dim conn As New ADODB.Connection
    conn.ConnectionString = "Driver={MySQL ODBC 8.0 Unicode Driver};Data Source=VBmySQL;Server=localhost;Database=booksys;User=root;Password=662258;Option=3"
    conn.Open '建立连接
    
    Dim rs As New ADODB.Recordset
    Set rs.ActiveConnection = conn
    rs.CursorLocation = 3
    
    '验证用户信息
    rs.Open "SELECT * FROM book WHERE b_No='" & out_book.Text & "'"
    If rs.RecordCount = 0 Then
        Label6.Caption = "图书编号不存在"
        Label6.Visible = True
        rs.Close
        conn.Close
        Exit Sub
    End If
    
    '编号有效
    Dim rs1 As New ADODB.Recordset
    Set rs1.ActiveConnection = conn
    rs1.CursorLocation = 3
    
    rs1.Open "SELECT * FROM borrow WHERE b_no='" & out_book.Text & "'"
    Dim size As Integer
    size = rs1.RecordCount
    
    l_output.Caption = "编号为" & out_book.Text & "的图书共借出" & Str(size) & "本, 余" & rs.Fields("storage") & "本"

    '在list中显示所有读者编号
    For i = 1 To size Step 1
        List1.AddItem "读者" & Str(i) & " :" & rs1.Fields("c_no") & "  应还:" & rs1.Fields("returntime")
        rs1.MoveNext
    Next
    
End Sub

Private Sub Command4_Click() '还书
    Call ini(1)
    Dim isErr As Integer
    isErr = 0
    
    If Len(in_reader.Text) = 0 Then
        Label7.Caption = "输入不可为空"
        Label7.Visible = True
        isErr = 1
    End If
    
    If Len(in_book.Text) = 0 Then
        Label8.Caption = "输入不可为空"
        Label8.Visible = True
        isErr = 1
    End If
    
    '有空输入则退出
    If isErr = 1 Then Exit Sub
    
    '检查借阅信息是否存在
    Dim ck As Integer
    ck = checkcase()
    If ck = 0 Then
        Label8.Caption = "借阅信息不存在"
        Label8.Visible = True
        Exit Sub
    End If
    
    
    '建立连接
    Dim conn As New ADODB.Connection
    conn.ConnectionString = "Driver={MySQL ODBC 8.0 Unicode Driver};Data Source=VBmySQL;Server=localhost;Database=booksys;User=root;Password=662258;Option=3"
    conn.Open
    '删除borrow中的借阅关系
    Dim del_bor As New ADODB.Recordset
    Set del_bor.ActiveConnection = conn
    del_bor.CursorLocation = 3
    del_bor.Open "DELETE FROM borrow WHERE b_no='" & in_book.Text & "' AND c_no='" & in_reader.Text & "'"
    'book中storage+1
    Dim add_sto As New ADODB.Recordset
    Set add_sto.ActiveConnection = conn
    add_sto.CursorLocation = 3
    add_sto.Open "UPDATE book SET storage=storage+1 WHERE b_No='" & in_book.Text & "'"
    conn.Close
    Label10.Visible = True
    Call showborrow
End Sub

Private Sub Form_Load()
    Call ini(0)
End Sub

Function showborrow() As Integer
    Dim conn As New ADODB.Connection
    conn.ConnectionString = "Driver={MySQL ODBC 8.0 Unicode Driver};Data Source=VBmySQL;Server=localhost;Database=booksys;User=root;Password=662258;Option=3"
    conn.Open '建立连接
     '显示读者当前借阅信息
    Dim cur As New ADODB.Recordset
    Set cur.ActiveConnection = conn
    cur.CursorLocation = 3
    
    cur.Open "SELECT * FROM borrow WHERE c_no='" & in_reader.Text & "'"
    Dim size As Integer
    size = cur.RecordCount
    
    l_output.Caption = "编号为" & in_reader.Text & "的读者目前共借阅了" & Str(size) & "本书"
    
    Dim cur_bookno As String '当前图书编号
    Dim sql_book_info As ADODB.Recordset
    '在list中显示该编号读者借阅的图书编号+书名
    For i = 1 To size Step 1
        cur_bookno = cur.Fields("b_no")
        
        Set sql_book_info = New ADODB.Recordset
        Set sql_book_info.ActiveConnection = conn
        sql_book_info.Open "SELECT * FROM book WHERE b_No='" & cur_bookno & "'"
        List1.AddItem cur.Fields("returntime") & "  " & sql_book_info.Fields("b_No") & "  " & sql_book_info.Fields("bookname")
        sql_book_info.Close
        
        cur.MoveNext
    Next
    conn.Close
End Function

Function checkcase() As Integer
    '建立连接
    Dim conn As New ADODB.Connection
    conn.ConnectionString = "Driver={MySQL ODBC 8.0 Unicode Driver};Data Source=VBmySQL;Server=localhost;Database=booksys;User=root;Password=662258;Option=3"
    conn.Open
        
    '判断是否存在借阅关系
    Dim check_group As New ADODB.Recordset
    Set check_group.ActiveConnection = conn
    check_group.CursorLocation = 3
    check_group.Open "SELECT * FROM borrow WHERE b_no='" & in_book.Text & "' AND c_no='" & in_reader.Text & "'"
   
    If check_group.RecordCount = 0 Then '借阅信息不存在
        checkcase = 0
    Else
        checkcase = 1
    End If
    
    conn.Close
End Function
