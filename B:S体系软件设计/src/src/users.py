from operator import or_

from flask import Blueprint, request, jsonify, session, redirect, g, render_template
from src.database import db
from src.model import User, Area
from sqlalchemy.orm import sessionmaker

# 定义二级路由
users = Blueprint('users', __name__)


@users.before_request
def before_request():
    g.user = 'user_id' in session


# 用户登录
@users.route('/login', methods=['POST'])
def login():
    session.pop('user_id', None)

    username = request.form.get("username", None)
    password = request.form.get("password", None)

    db_session = sessionmaker(bind=db)()
    res = db_session.query(User).filter(User.name == username).all()
    if not res:
        return jsonify({'status': 503})  # 用户不存在
    else:
        if res[0].password != password:
            return jsonify({'status': 504})  # 密码错误
        else:
            session['user_id'] = res[0].id
            session.pop('user_name', None)
            session.pop('area_id', None)
            session.pop('g_path', None)
            session.pop('area_name', None)
            # get name
            db_session = sessionmaker(bind=db)()
            g.user = res[0].id
            res = db_session.query(User).filter(User.id == g.user).first()
            session['user_name'] = res.name
            # get init area_id
            res = db_session.query(Area).filter(Area.user_id == g.user).order_by(Area.id).first()
            if res:
                session['area_id'] = res.id
                session['area_name'] = res.name
                # get init area_bgi_path
                session['g_path'] = './media/' + res.graph + '.png'
            else:
                session['area_id'] = -1
                session['area_name'] = "None"
                session['g_path'] = ""

            return jsonify({'status': 200})


# 用户登出
@users.route('/logout', methods=['POST'])
def logout():
    session.pop('user_id', None)
    return jsonify({
        'status': 200,
        'data': render_template("login.html")
    })


# 用户注册
@users.route('/signup', methods=["POST"])
def sign_up():
    session.pop('user_id', None)

    username = request.form.get("username", None)
    phone = request.form.get("phone", None)
    password = request.form.get("password", None)

    # 验证 unique
    db_session = sessionmaker(bind=db)()
    res = db_session.query(User).filter(or_(User.name == username, User.phone == phone)).all()

    if res:
        # 记录重复
        if res[0].name == username:
            return jsonify({'status': 501})  # 用户名已存在
        else:
            return jsonify({'status': 502})  # 手机号已存在

    # 新建账号
    usr = User(username, password, phone)
    db_session.add(usr)
    db_session.commit()

    # 获取id
    res = db_session.query(User).filter(User.phone == phone).all()
    db_session.close()

    session['user_id'] = res[0].id
    session.pop('user_name', None)
    session.pop('area_id', None)
    session.pop('g_path', None)
    session.pop('area_name', None)
    # get name
    db_session = sessionmaker(bind=db)()
    g.user = res[0].id
    res = db_session.query(User).filter(User.id == g.user).first()
    session['user_name'] = res.name
    # get init area_id
    res = db_session.query(Area).filter(Area.user_id == g.user).order_by(Area.id).first()
    if res:
        session['area_id'] = res.id
        session['area_name'] = res.name
        # get init area_bgi_path
        session['g_path'] = './media/' + res.graph + '.png'
    else:
        session['area_id'] = -1
        session['area_name'] = "None"
        session['g_path'] = ""

    return jsonify({'status': 200})
