from flask import Blueprint, request, jsonify, g, session
from src.database import db
from src.model import Room, Equipment
from sqlalchemy.orm import sessionmaker

# 定义二级路由
rooms = Blueprint('rooms', __name__)


@rooms.before_request
def before_request():
    # g.user = 'user_id' in session
    g.user = 1 #id
    g.aid = session['area_id']


@rooms.route('/', methods=['POST'])
def test():
    if g.aid == -1:
        return jsonify({"status": 200, "data": []})

    db_session = sessionmaker(bind=db)()
    res = db_session.query(Room).filter(Room.area_id == g.aid).all()
    data = []
    for r in res:
        e_res = db_session.query(Equipment).filter(Equipment.room_id == r.id).all()
        e_list = []
        for e in e_res:
            e_list.append({"name": e.name, "type": e.type, "status": e.status, "info": e.info, "id": e.id, "x": e.pos_x, "y": e.pos_y})
        data.append({"name": r.name, "id": r.id, "list": e_list})

    return jsonify({"status": 200, "data": data})


@rooms.route('/add', methods=['POST'])
def add_room():
    room_name = request.form.get("name", None)
    area_id = request.form.get("aid", None)

    db_session = sessionmaker(bind=db)()
    # add room
    neo_room = Room(room_name, area_id)
    db_session.add(neo_room)
    db_session.commit()

    res = db_session.query(Room).filter(Room.area_id == g.user).all()
    data = []
    for r in res:
        e_res = db_session.query(Equipment).filter(Equipment.room_id == r.id).all()
        e_list = []
        for e in e_res:
            e_list.append({"name": e.name, "type": e.type, "status": e.status, "info": e.info, "id": e.id})
        data.append({"name": r.name, "id": r.id, "list": e_list})

    return jsonify({"status": 200, "data": data})
