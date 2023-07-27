-- 删除已存在的 database distributed
DROP DATABASE IF EXISTS `distributed`;

-- 创建 database distributed
CREATE DATABASE distributed DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

-- 创建数据表
USE distributed;

CREATE TABLE user ( 
    user_id int(10) primary key Auto_Increment, 
    username varchar(20) not null,
    password varchar(20) not null
);

-- 初始化数据
INSERT INTO user (username, password) VALUES
    ('admin', '123456'),
    ('sam', '666666'),
    ('jack', '888888'),
    ('test', '123');