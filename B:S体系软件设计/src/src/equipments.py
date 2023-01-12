from flask import Blueprint, request, jsonify
from src.database import db
from src.model import Room, Equipment
from sqlalchemy.orm import sessionmaker
from sqlalchemy import and_

# 定义二级路由
equips = Blueprint('equips', __name__)


@equips.route('/new', methods=['POST'])
def add_equip():
    area_id = 1;
    room_name = request.form.get("rname", None)
    equip_name = request.form.get("dname", None)
    equip_type = request.form.get("dtype", None)
    db_session = sessionmaker(bind=db)()
    # room exists?
    room = db_session.query(Room).filter(and_(Room.name == room_name, Room.area_id == area_id)).all()
    if not room:
        return jsonify({"status": 505}) # 房间不存在
    # add equip
    room_id = room[0].id
    neo_equip = Equipment(equip_name, equip_type, room_id)
    db_session.add(neo_equip)
    db_session.commit()
    # return new status
    res = db_session.query(Room).filter(Room.area_id == 1).all()
    data = []
    for r in res:
        e_res = db_session.query(Equipment).filter(Equipment.room_id == r.id).all()
        e_list = []
        for e in e_res:
            e_list.append({"name": e.name, "type": e.type, "status": e.status, "info": e.info, "id": e.id, "x": e.pos_x, "y": e.pos_y})
        data.append({"name": r.name, "id": r.id, "list": e_list})
    return jsonify({"status": 200, "data": data})


@equips.route('/status', methods=['POST'])
def mod_status():
    equip_id = request.form.get("eid", None)
    neo_status = request.form.get("status", None)

    db_session = sessionmaker(bind=db)()
    db_session.query(Equipment).filter(Equipment.id == equip_id).update({"status": neo_status})
    db_session.commit()
    return jsonify({"status": 200})


@equips.route('/info', methods=['POST'])
def mod_info():
    equip_id = request.form.get("eid", None)
    neo_info = request.form.get("info", None)
    db_session = sessionmaker(bind=db)()
    db_session.query(Equipment).filter(Equipment.id == equip_id).update({"info": neo_info})
    db_session.commit()
    return jsonify({"status": 200})


@equips.route('pos', methods=['POST'])
def save_pos():
    # str to list
    ids = request.form.get("ids", None).strip('[').strip(']').split(',')
    xlist = request.form.get("xs", None).strip('[').strip(']').split(',')
    ylist = request.form.get("ys", None).strip('[').strip(']').split(',')

    db_session = sessionmaker(bind=db)()
    idx = 0
    for id in ids:
        pos_x = xlist[idx]
        pos_y = ylist[idx]
        db_session.query(Equipment).filter(Equipment.id == id).update({"pos_x": pos_x, "pos_y": pos_y})
        idx += 1
    db_session.commit()

    return jsonify({"status": 200})
