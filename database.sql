-- 大学生小组课题选题管理系统数据库设计

-- 用户表：存储管理员、教师、学生小组账号信息
CREATE TABLE users (
    user_id INT AUTO_INCREMENT PRIMARY KEY COMMENT '用户ID',
    username VARCHAR(50) NOT NULL UNIQUE COMMENT '账号',
    password VARCHAR(255) NOT NULL COMMENT '密码（MD5加密）',
    user_type TINYINT NOT NULL COMMENT '用户类型：1-管理员，2-教师，3-学生小组',
    real_name VARCHAR(100) NOT NULL COMMENT '真实姓名/小组名称',
    email VARCHAR(100) COMMENT '邮箱',
    phone VARCHAR(20) COMMENT '联系电话',
    status TINYINT DEFAULT 1 COMMENT '状态：1-正常，0-禁用',
    create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    update_time DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='用户表';

-- 教师信息表：存储教师详细信息
CREATE TABLE teachers (
    teacher_id INT AUTO_INCREMENT PRIMARY KEY COMMENT '教师ID',
    user_id INT NOT NULL COMMENT '关联用户ID',
    department VARCHAR(100) COMMENT '所属院系',
    title VARCHAR(50) COMMENT '职称',
    research_area TEXT COMMENT '研究方向',
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='教师信息表';

-- 学生小组信息表：存储学生小组详细信息
CREATE TABLE student_groups (
    group_id INT AUTO_INCREMENT PRIMARY KEY COMMENT '小组ID',
    user_id INT NOT NULL COMMENT '关联用户ID',
    leader_name VARCHAR(100) NOT NULL COMMENT '组长姓名',
    member_count INT DEFAULT 1 COMMENT '成员人数',
    members TEXT COMMENT '成员列表（JSON格式）',
    major VARCHAR(100) COMMENT '专业',
    grade VARCHAR(20) COMMENT '年级',
    class_name VARCHAR(100) COMMENT '班级',
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='学生小组信息表';

-- 课题表：存储管理员发布的课题信息
CREATE TABLE topics (
    topic_id INT AUTO_INCREMENT PRIMARY KEY COMMENT '课题ID',
    topic_name VARCHAR(200) NOT NULL UNIQUE COMMENT '课题名称',
    teacher_id INT COMMENT '指导教师ID',
    description TEXT COMMENT '课题描述',
    requirements TEXT COMMENT '选题要求',
    max_students INT NOT NULL DEFAULT 5 COMMENT '最大小组人数',
    current_students INT DEFAULT 0 COMMENT '当前选择人数',
    status TINYINT DEFAULT 1 COMMENT '状态：1-可选题，0-已关闭',
    create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    update_time DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    FOREIGN KEY (teacher_id) REFERENCES teachers(teacher_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='课题表';

-- 选题申请表：存储学生小组的选题申请信息
CREATE TABLE topic_applications (
    application_id INT AUTO_INCREMENT PRIMARY KEY COMMENT '申请ID',
    group_id INT NOT NULL COMMENT '小组ID',
    topic_id INT NOT NULL COMMENT '课题ID',
    application_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '申请时间',
    status TINYINT DEFAULT 0 COMMENT '申请状态：0-待审核，1-已通过，2-已驳回',
    reject_reason TEXT COMMENT '驳回原因',
    review_time DATETIME COMMENT '审核时间',
    reviewer_id INT COMMENT '审核人ID',
    FOREIGN KEY (group_id) REFERENCES student_groups(group_id) ON DELETE CASCADE,
    FOREIGN KEY (topic_id) REFERENCES topics(topic_id) ON DELETE CASCADE,
    FOREIGN KEY (reviewer_id) REFERENCES users(user_id) ON DELETE SET NULL,
    UNIQUE KEY unique_application (group_id, topic_id) COMMENT '一个小组只能申请一个课题'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='选题申请表';

-- 系统配置表：存储系统全局配置信息
CREATE TABLE system_config (
    config_id INT AUTO_INCREMENT PRIMARY KEY COMMENT '配置ID',
    config_key VARCHAR(100) NOT NULL UNIQUE COMMENT '配置键',
    config_value TEXT COMMENT '配置值',
    config_desc VARCHAR(200) COMMENT '配置描述',
    create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    update_time DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='系统配置表';

-- 初始化数据：默认管理员账号
INSERT INTO users (username, password, user_type, real_name, email, phone) 
VALUES ('admin', MD5('admin123'), 1, '系统管理员', 'admin@example.com', '13800138000');

-- 初始化系统配置
INSERT INTO system_config (config_key, config_value, config_desc) 
VALUES ('system_name', '大学生小组课题选题管理系统', '系统名称'),
       ('system_version', '1.0.0', '系统版本'),
       ('topic_apply_start_time', '2024-01-01 00:00:00', '选题申请开始时间'),
       ('topic_apply_end_time', '2024-12-31 23:59:59', '选题申请结束时间');

-- 创建索引以提高查询性能
CREATE INDEX idx_users_user_type ON users(user_type);
CREATE INDEX idx_topics_status ON topics(status);
CREATE INDEX idx_topic_applications_status ON topic_applications(status);
CREATE INDEX idx_topic_applications_group_id ON topic_applications(group_id);