from sqlalchemy import create_engine
import os

# 进行数据库配置 mysql+pymysql://用户名:密码@服务器:端口号/数据库名称
DB_URI = 'mysql+pymysql://root:662258@localhost:3306/bs'

db = create_engine(DB_URI)

BASE_DIR = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))
MEDIA_DIR = os.path.join(BASE_DIR, 'static/media/')
