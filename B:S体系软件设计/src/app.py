from flask import Flask, render_template, g, redirect, url_for, session
from src.database import db
from sqlalchemy.orm import sessionmaker
from src.model import User, Area
from src.users import users
from src.area import areas
from src.rooms import rooms
from src.equipments import equips
from flask_cors import CORS # 支持跨域

app = Flask(__name__, static_url_path="/")
CORS(app, supports_credentials=True)
app.secret_key = "test"

app.register_blueprint(users, url_prefix='/user')
app.register_blueprint(rooms, url_prefix='/room')
app.register_blueprint(equips, url_prefix='/equip')
app.register_blueprint(areas, url_prefix='/area')

@app.before_request
def before_request():
    # g.user = 'user_id' in session
    if 'user_id' in session:
        g.user = session['user_id']
    else:
        g.user = None
    if 'user_name' in session:
        g.name = session['user_name']
        g.aid = session['area_id']
        g.aname = session['area_name']
        g.path = session['g_path']
    else:
        g.name = ""
        g.aid = -1
        g.aname = ""
        g.path = ""

# 显示登录页
@app.route('/', methods=['GET'])
def index():
    return render_template("login.html")


@app.route('/mobile', methods=['GET'])
def mobile_index():
    return render_template("mobile_login.html")


@app.route('/next')
def next_scene():
    return render_template("profile.html")


# 显示主页
@app.route('/mobile_profile')
def mobile_profile():
    if not g.user:
        return redirect(url_for('mobile_index'))
    return render_template("mobile_profile.html")


@app.route('/profile')
def profile():
    if not g.user:
        return redirect(url_for('index'))
    return render_template("profile.html")


if __name__ == '__main__':
    # 后端项目运行在 8800 端口
    app.run(debug=True, host="0.0.0.0", port=8080)
