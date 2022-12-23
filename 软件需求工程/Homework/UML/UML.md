# UML-1-沈韵沨-3200104392

> SAMPLE - **"社团纳新平台"**
>
> 使用的作图软件为：StarUML，附源文件

## Actors

1. 管理员
   需要登陆，可以对面试官账户进行管理，可以对平台上的纳新实例进行管理
2. 面试官
   需要登陆，可以创建纳新实例并对同社团其他面试官创建的实例进行管理，可以对纳新实例下的面试者状态进行管理，可以通过平台向面试者发送通知短信。
3. 面试者
   无需登录，可以查看并报名平台上存在的纳新实例，可以通过手机号查询自己当前的面试状态。

## Use Cases

### 1 （手动）添加用户

* Actor：管理员

* Description：管理员手动向系统添加用户

* Trigger：管理员选择“添加用户”功能

* Preconditions：管理员已登录

* Postconditions：使用初始密码的新用户被成功添加

* Normal Flow：

  1. 管理员从已存在社团列表中为新用户选取所属社团
  2. 管理员输入用户名、用户学号等基本信息
  3. 系统创建用户
  4. 系统返回“创建成功”提示

* Alternative Flow：
  A) 用户所属社团在社团列表中不存在时-1

  1. 更改为：管理员在已存在社团列表中选择“（新建）”选项，并输入新社团名称

  B) 用户所属社团在社团列表中不存在时-2

  ​	若管理员在 (1) 中输入的社团名已存在，则：

  2. 系统提示“社团名已存在，请重新输入”

  C) 用户学号在系统中已存在-1

  	4. 更改为：系统返回“用户已存在，是否为其添加新组织”提示
  	4. 管理员选择“是”
  	4. 系统为该用户增加新的所属社团信息
  	4. 系统返回“用户信息更新成功”提示

  D）用户学号在系统中已存在-2

  ​	类似于 Alternative Flow - C，若管理员在 (5) 中选择了“否”，则：

  6. 系统返回“已取消对该用户的更改”提示

* Exceptions：网络错误

* Priority：中

* Frequency of Use：低

* Bussiness Rules：学号是用户在系统中的唯一标识，同账户可同时从属于多个社团

* Other Information：无

* Assumption：无

### 2 删除用户

* Actor：管理员
* Description：管理员从用户列表中删除已存在的用户
* Trigger：管理员选择“删除用户”功能
* Preconditions：管理员已登录
* Postconditions：指定用户从用户列表中删除
* Normal Flow：
  1. 管理员输入待删除用户学号
  2. 系统移除该账号
  3. 系统返回“删除成功”提示
* Alternative Flow：
  1. 也可以为：管理员从用户列表中勾选一个或多个待删除用户
* Exceptions：网络错误
* Priority：低
* Frequency of Use：低
* Bussiness Rules：无
* Other Information：无
* Assumption：无

### 3 发布纳新信息

* Actor：面试官
* Description：面试官在平台上发布纳新信息
* Trigger：面试官选择“发布纳新信息”功能
* Preconditions：面试官已登录
* Postconditions：新的面试实例被创建，并在平台主页显示相关信息
* Normal Flow：
  1. 面试官输入纳新实例信息、起止时间、表单信息及其他相关信息
  2. 系统创建纳新实例
  3. 系统返回“发布成功”
* Alternative Flow：无
* Exceptions：网络错误；起止时间错误
* Priority：高
* Frequency of Use：中
* Bussiness Rules：面试开始时间 < 面试结束时间
* Other Information：无
* Assumption：无

### 4 删除纳新信息

* Actor：面试官/管理员
* Description：删除平台上已存在的纳新实例
* Trigger：用户选择“删除纳新信息”功能
* Preconditions：用户已登录
* Postconditions：纳新实例被删除，相关信息被移除
* Normal Flow：
  1. 用户在纳新实例列表中选择待删除的实例
  2. 系统删除纳新实例
  3. 系统返回“删除成功”提示
* Alternative Flow：无
* Exceptions：网络错误；多个用户同时删除同一纳新实例
* Priority：低
* Frequency of Use：低
* Bussiness Rules：无
* Other Information：无
* Assumption：无

### 5 查看纳新信息

* Actor：面试官/面试者
* Description：用户查看平台上所有正在进行的纳新信息
* Trigger：用户选择“查看纳新信息”功能
* Preconditions：无
* Postconditions：系统显示纳新信息聚合页
* Normal Flow：
  1. 用户选择“查看纳新信息”功能
  2. 系统返回纳新信息聚合页
* Alternative Flow：无
* Exceptions：网络错误
* Priority：高
* Frequency of Use：高
* Bussiness Rules：无
* Other Information：无
* Assumption：无

### 6 用户注册

* Actor：面试官
* Description：没有平台账号的面试官主动发起账号注册
* Trigger：用户选择“注册”功能
* Preconditions：无
* Postconditions：成功创建新的面试官账号
* Normal Flow：
  1. 用户打开平台主页，选择“注册”功能
  2. 用户选择从社团列表中选择自己所属的社团，输入其他基本注册信息
  3. 系统创建账号
  4. 系统返回“注册成功”信息，并跳转至后台管理页面
* Alternative Flow：无
* Exceptions：网络错误
* Priority：中
* Frequency of Use：低
* Bussiness Rules：无
* Other Information：无
* Assumption：无

### 7 用户登录

* Actor：管理员/面试官
* Description：用户登录平台
* Trigger：用户选择“登录”功能
* Preconditions：无
* Postconditions：用户成功登录，跳转至后台管理页面
* Normal Flow：
  1. 用户打开平台主页，选择“登录”功能
  2. 用户输入账号密码
  3. 系统进行身份验证
  4. 系统返回“登陆成功”
* Alternative Flow：无
* Exceptions：网络错误；账号或密码错误；账号不存在
* Priority：高
* Frequency of Use：高
* Bussiness Rules：面试官以学号为账号进行登录，管理员以预置账号密码登录
* Other Information：无
* Assumption：无

### 8 发送通知信息

* Actor：面试官
* Description：面试官以短信形式通知面试者时间地点/面试结果
* Trigger：面试官选择“发送通知“功能
* Preconditions：用户已登录
* Postconditions：平台自动发送通知短信
* Normal Flow：
  1. 面试官选择面试实例
  2. 面试官从面试者列表中选取待发送通知的面试者
  3. 面试官编辑短信文案
  4. 系统批量发送短信
  5. 系统返回“发送成功”信息
* Alternative Flow：无
* Exceptions：网络错误；部分短信发送失败
* Priority：中
* Frequency of Use：中
* Bussiness Rules：无
* Other Information：无
* Assumption：无

### 9 修改面试者状态

* Actor：面试官
* Description：面试官根据面试者目前所处的面试轮次/面试结果对面试者状态进行修改
* Trigger：面试官选择“修改面试者信息”
* Preconditions：用户已登录
* Postconditions：面试者状态被修改为指定状态
* Normal Flow：
  1. 面试官选取面试实例
  2. 面试官从面试者列表中选取待修改状态的面试者
  3. 面试官选取面试者新状态
  4. 系统修改面试者状态
  5. 系统返回“修改成功”信息
* Alternative Flow：无
* Exceptions：网络错误；修改失败
* Priority：高
* Frequency of Use：高
* Bussiness Rules：不同面试实例中的同一面试者（以手机号唯一标识）可以处于不同的状态
* Other Information：无
* Assumption：无

### 10 查看面试者状态

* Actor：面试官/面试者

* Description：用户查看自己在某一纳新示例下的状态（面试官可查看某一纳新实例下的所有面试者装填）

* Trigger：用户选择“查看面试状态“功能

* Preconditions：用户已登录（面试者无需登录）

* Postconditions：系统返回面试者状态（对于面试官，系统返回该纳新实例下所有面试者状态）

* Normal Flow：

  （面试者）

  1. 面试者打开平台主页
  2. 面试者选择”查询面试状态“功能
  3. 面试者输入手机号
  4. 系统查询对应面试者信息
  5. 系统返回查询结果页面

  （面试官）

  1. 面试官选取纳新实例
  2. 面试官选择”查看面试者状态“功能
  3. 系统查询面试者信息
  4. 系统返回查询结果页面

* Alternative Flow：无

* Exceptions：网络错误；查询失败

* Priority：低

* Frequency of Use：中

* Bussiness Rules：无

* Other Information：无

* Assumption：无

### 11 报名纳新实例

* Actor：面试者
* Description：面试者报名平台上正在进行的纳新实例
* Trigger：面试者选择”报名“功能
* Preconditions：无
* Postconditions：面试者成功报名纳新
* Normal Flow：
  1. 面试者进入平台首页查看正在进行的纳新实例
  2. 面试者选择待报名的纳新实例，选择"报名"
  3. 系统返回对应实例的报名表单
  4. 面试者填写表单信息
  5. 系统提交表单
  6. 系统返回”报名成功“信息
* Alternative Flow：无
* Exceptions：网络错误；用户填写表单时，纳新实例已截止
* Priority：高
* Frequency of Use：高
* Bussiness Rules：手机号将作为面试者的唯一标识
* Other Information：无
* Assumption：无

## Use Case Diagram

![](imgs\USE_CASE_DIAGRAM.jpg)

## Sequence Diagram & Collaboration Diagram 

> 因为 StarUML中缺少 Collaboration Diagram 选项，此处使用 drow.io 完成

### 1 添加用户

![sd1](imgs\sd1.jpg)

![cd1](imgs\cd1.jpg)

### 2 查看面试者状态(面试者)

![sd2](imgs\sd2.jpg)

![cd2](imgs\cd2.jpg)

### 3 修改面试者状态

![sd3](imgs\sd3.jpg)

![cd3](imgs\cd3.jpg)

### 4 发布纳新信息

![sd4](imgs\sd4.jpg)

![cd4](imgs\cd4.jpg)
