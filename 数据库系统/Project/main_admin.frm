VERSION 5.00
Begin VB.Form Main_Admin 
   Caption         =   "个人主页"
   ClientHeight    =   4500
   ClientLeft      =   6930
   ClientTop       =   5670
   ClientWidth     =   9435
   LinkTopic       =   "Form1"
   ScaleHeight     =   4500
   ScaleWidth      =   9435
   Begin VB.CommandButton C_Logout 
      Caption         =   "登出"
      Height          =   495
      Left            =   7680
      TabIndex        =   9
      Top             =   3840
      Width           =   1575
   End
   Begin VB.CommandButton C_Manage 
      Caption         =   "借书证管理"
      Height          =   495
      Left            =   7680
      TabIndex        =   8
      Top             =   2400
      Width           =   1575
   End
   Begin VB.CommandButton C_Load 
      Caption         =   "入库"
      Height          =   495
      Left            =   7680
      TabIndex        =   7
      Top             =   1680
      Width           =   1575
   End
   Begin VB.CommandButton C_Borrow 
      Caption         =   "借还书"
      Height          =   495
      Left            =   7680
      TabIndex        =   6
      Top             =   960
      Width           =   1575
   End
   Begin VB.CommandButton C_Query 
      Caption         =   "查询"
      Height          =   495
      Left            =   7680
      TabIndex        =   5
      Top             =   240
      Width           =   1575
   End
   Begin VB.Label Label4 
      Caption         =   "请点击右侧按钮进行管理"
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
      Left            =   360
      TabIndex        =   4
      Top             =   1560
      Width           =   3615
   End
   Begin VB.Label Label3 
      Caption         =   "管理员"
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
      Left            =   2280
      TabIndex        =   3
      Top             =   960
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
      Left            =   360
      TabIndex        =   2
      Top             =   960
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
      Left            =   1320
      TabIndex        =   1
      Top             =   360
      Width           =   1935
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
      Left            =   360
      TabIndex        =   0
      Top             =   360
      Width           =   975
   End
End
Attribute VB_Name = "Main_Admin"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Form_Load()
    Label1.Caption = Login.u_name '在主页显示用户名
End Sub

Private Sub c_query_Click() '前往图书查询界面
    'Main_Admin.Hide
    F_Query.Show
End Sub

Private Sub C_Borrow_Click() '前往借还书界面
    'Main_Admin.Hide
    F_BorR.Show
End Sub


Private Sub C_Load_Click() '前往入库界面
    'Main_Admin.Hide
    Call F_Load.inilabel
    Call F_Load.display_all
    F_Load.Show
End Sub

Private Sub C_Manage_Click() '前往借书证管理界面
    'Main_Admin.Hide
    F_Manage.Show
End Sub

Private Sub C_Logout_Click() '登出按钮
    Login.user_type = 0
    '关闭所有子页面
    F_BorR.Hide
    F_Load.Hide
    F_Manage.Hide
    F_Query.Hide
    neo_reader.Hide
        
    Dim logout_status, tmp
    logout_status = MsgBox("要返回登录界面吗？", vbYesNo, "图书管理系统")
    
    If logout_status = vbYes Then
        Main_Admin.Hide
        Login.Show
        Login.user_type = 0
    Else
        tmp = MsgBox("感谢使用, 已成功登出！", vbInformation, "图书管理系统")
        Main_Admin.Hide
        End
    End If
End Sub


