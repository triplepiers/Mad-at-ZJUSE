from operator import and_

from flask import Blueprint, request, jsonify, session, redirect, g, render_template, current_app
from src.database import db, MEDIA_DIR
from sqlalchemy.orm import sessionmaker
from src.model import Area
from uuid import uuid4
import base64

areas = Blueprint('areas', __name__)

@areas.before_request
def before_request():
    # g.user = 'user_id' in session
    g.user = 1
    g.name = session['user_name']
    g.aid = session['area_id']
    g.aname = session['area_name']
    g.path = session['g_path']
# @areas.route('/', methods=['POST'])
# def save_img():
#     img = request.form.get("img")
#     img_base64 = base64.b64decode(img)
#
#     filename = MEDIA_DIR + str(uuid4()) + '.png'  # 为了生成一个不重复的 文件名
#     # 将图片写到本地
#     with open(filename, "wb") as f:
#         f.write(img_base64)
#     # return filename
#     return "ok"


@areas.route('/next', methods=['POST'])
def next_area():
    db_session = sessionmaker(bind=db)()
    res = db_session.query(Area).filter(and_(Area.user_id == g.user, Area.id > g.aid)).order_by(Area.id).first()
    if res:
        session['area_id'] = res.id
        session['area_name'] = res.name
        session['g_path'] = './media/' + res.graph + '.png'
    else:
        session['area_id'] = -1
        session['area_name'] = "None"
        session['g_path'] = ""
    return jsonify({"status": 200})


@areas.route('/prev', methods=['POST'])
def prev_area():
    db_session = sessionmaker(bind=db)()
    res = db_session.query(Area).filter(and_(Area.user_id == g.user, Area.id < g.aid)).order_by(Area.id.desc()).first()
    if res:
        session['area_id'] = res.id
        session['area_name'] = res.name
        session['g_path'] = './media/' + res.graph + '.png'
    else:
        session['area_id'] = -1
        session['area_name'] = "None"
        session['g_path'] = ""

    return jsonify({"status": 200})


@areas.route('/add', methods=['POST'])
def neo_area():
    g.user = 1
    area_name = request.form.get("aname")
    img = request.form.get("img")
    # save pic
    img_base64 = base64.b64decode(img)
    f_name = str(uuid4())
    filename = MEDIA_DIR + f_name + '.png'  # 为了生成一个不重复的 文件名
    # 将图片写到本地
    with open(filename, "wb") as f:
        f.write(img_base64)
    # create area
    db_session = sessionmaker(bind=db)()
    neo_area = Area(area_name, f_name, g.user)
    db_session.add(neo_area)
    db_session.commit()

    return jsonify({"status": 200})