from sqlalchemy import Column, String, Integer
from sqlalchemy.ext.declarative import declarative_base

Base = declarative_base()
meta = Base.metadata


# user 表
class User(Base):
    __tablename__ = "user"

    def __init__(self, name, password, phone):
        self.name = name
        self.password = password
        self.phone = phone

    id = Column(Integer, primary_key=True)
    name = Column(String(20), nullable=False, unique=True)
    password = Column(String(15), nullable=False)
    phone = Column(String(11), nullable=False, unique=True)


# area 表
class Area(Base):
    __tablename__ = "area"

    def __init__(self, name, graph, user_id):
        self.name = name
        self.graph = graph
        self.user_id = user_id

    id = Column(Integer, primary_key=True)
    name = Column(String(15), nullable=False, unique=True)
    graph = Column(String(50), nullable=False)
    user_id = Column(Integer, nullable=False)

# room 表
class Room(Base):
    __tablename__ = "room"

    def __init__(self, name, area_id):
        self.name = name
        self.area_id = area_id

    name = Column(String(15), nullable=False)
    id = Column(Integer, primary_key=True)
    area_id = Column(Integer, nullable=False)


# equipment 表
class Equipment(Base):
    __tablename__ = "equipment"

    def __init__(self, name, type, room_id):
        self.name = name
        self.type = type
        self.status = 0
        self.info = "0"
        self.pos_x = 0
        self.pos_y = 0
        self.room_id = room_id

    name = Column(String(15), nullable=False)
    type = Column(Integer, nullable=False)
    status = Column(Integer, nullable=False)
    info = Column(String(50), nullable=False)
    pos_x = Column(Integer, nullable=False)
    pos_y = Column(Integer, nullable=False)
    id = Column(Integer, primary_key=True)
    room_id = Column(Integer, nullable=False)
