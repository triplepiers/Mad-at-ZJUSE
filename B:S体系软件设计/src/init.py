import pymysql

info = pymysql.connect(
    host='127.0.0.1',  # 数据库地址
    port=3306,  # 数据库端口
    user='root',  # 数据库账号
    password='662258'  # 数据库密码
    #db='bs'  # 数据库名
)

# 创建数据库对象
db = info.cursor()

# 若本地不存在 bs 数据库，请注释下面的两行代码
sql = "DROP database bs;"
db.execute(sql)

# 数据库采用 utf-8 字符集
sql = "CREATE DATABASE bs DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;"
db.execute(sql)

sql = "use bs;"
db.execute(sql)

# 创建 user 表
sql = "create table user(name varchar(20) not null unique, password varchar(20) not null, phone char(11) not null unique, id int(10) primary key auto_increment);"
db.execute(sql)

# 插入默认用户信息
sql = "insert into user (name, password, phone) values ('test', '123', '13958952258');"
db.execute(sql)

# 创建 area 表
sql = "create table area(name varchar(15) not null, graph varchar(50) not null, id int(10) primary key auto_increment, user_id int(10) not null, foreign key(user_id) references user(id));"
db.execute(sql)

# 创建 room 表
sql = "create table room(name varchar(15) not null , id int(10) primary key auto_increment, area_id int(10) not null, foreign key(area_id) references area(id));"
db.execute(sql)

# 创建 equipment 表
sql = "create table equipment(name varchar(15) not null, type int(2) not null, status bool not null default false, info varchar(50) not null default '（空）', pos_x int(10) not null default 0, pos_y int(10) not null default 0, id int(10) primary key auto_increment, room_id int(10) not null, foreign key(room_id) references room(id));"
db.execute(sql)

db.close()
info.close()
print("database initialization finished...")