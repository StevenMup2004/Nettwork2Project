CREATE DATABASE ticketrail;
USE ticketrail;

CREATE TABLE users (
    user_id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(255),
    phone VARCHAR(255),
    email VARCHAR(255),
    password VARCHAR(255),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP
);

CREATE TABLE airlines (
    airline_id VARCHAR(255) PRIMARY KEY,
    name VARCHAR(255),
    contact_email VARCHAR(255),
    contact_phone VARCHAR(255),
    logo_url VARCHAR(255)
);

INSERT INTO airlines (airline_id, name) VALUES
('VN', 'Vietnam Airlines'),
('VJ', 'VietJet Air'),
('QH', 'Bamboo Airways'),
('V8', 'Vietravel Airlines');

CREATE TABLE airports (
    airport_id VARCHAR(255) PRIMARY KEY,
    name VARCHAR(255),
    city VARCHAR(255)
);

INSERT INTO airports (airport_id, name, city) VALUES
('SGN', 'Tân Sơn Nhất', 'Hồ Chí Minh'),
('HAN', 'Nội Bài', 'Hà Nội'),
('CXR', 'Cam Ranh', 'Khánh Hòa'),
('DAD', 'Đà Nẵng', 'Đà Nẵng'),
('HUI', 'Phú Bài', 'Thừa Thiên Huế'),
('UIH', 'Phù Cát', 'Bình Định'),
('VCL', 'Chu Lai', 'Quảng Nam'),
('VCS', 'Côn Đảo', 'Bà Rịa-Vũng Tàu'),
('THD', 'Trường Sa', 'Khánh Hòa'),
('DLI', 'Liên Khương', 'Lâm Đồng'),
('VKG', 'Quy Nhơn', 'Bình Định'),
('PQC', 'Phú Quốc', 'Kiên Giang'),
('VCA', 'Cần Thơ', 'Cần Thơ'),
('CAH', 'Cát Bi', 'Hải Phòng'),
('CJJ', 'Chu Lai', 'Quảng Nam');

CREATE TABLE airplanes (
    airplane_id VARCHAR(255) PRIMARY KEY,
    airline_id VARCHAR(255),
    price int,
    capacity INT,
    FOREIGN KEY (airline_id) REFERENCES airlines(airline_id) ON DELETE CASCADE
);

-- ĐÃ CẬP NHẬT: Giá vé đồng giá 10.000 VND
INSERT INTO airplanes (airplane_id, airline_id, price, capacity)
VALUES
    ('VN0001', 'VN', 10000, 150),
    ('VN0002', 'VN', 10000, 120),
    ('VN0003', 'VN', 10000, 180),
    ('VN0004', 'VN', 10000, 110),
    ('VN0005', 'VN', 10000, 200),

    ('VJ0001', 'VJ', 10000, 140),
    ('VJ0002', 'VJ', 10000, 130),
    ('VJ0003', 'VJ', 10000, 170),
    ('VJ0004', 'VJ', 10000, 160),
    ('VJ0005', 'VJ', 10000, 190),

    ('V8001', 'V8', 10000, 125),
    ('V8002', 'V8', 10000, 135),
    ('V8003', 'V8', 10000, 145),
    ('V8004', 'V8', 10000, 155),
    ('V8005', 'V8', 10000, 165),

    ('QH0001', 'QH', 10000, 100),
    ('QH0002', 'QH', 10000, 105),
    ('QH0003', 'QH', 10000, 110),
    ('QH0004', 'QH', 10000, 115),
    ('QH0005', 'QH', 10000, 120);

CREATE TABLE flights (
    flight_id varchar(255) PRIMARY KEY,
    departure_airport varchar(255),
    arrival_airport VARCHAR(255),
    departure_time DATETIME,
    duration_minutes INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP,
    airplane_id varchar(255),
    FOREIGN KEY (airplane_id) REFERENCES airplanes(airplane_id) ON DELETE CASCADE,
    FOREIGN KEY (departure_airport) REFERENCES airports(airport_id) ON DELETE CASCADE,
    FOREIGN KEY (arrival_airport) REFERENCES airports(airport_id) ON DELETE CASCADE
);

insert into flights (flight_id, departure_airport, arrival_airport, departure_time, duration_minutes, airplane_id)
values 
    ('VN001', 'SGN', 'HAN', '2024-12-13 17:00', 150, 'VN0001'),
    ('VN002', 'SGN', 'DAD', '2024-12-15 16:00', 170, 'VN0001'),
    ('VN003', 'SGN', 'HUI', '2024-12-17 16:00', 200, 'VN0001'),
    ('VN004', 'SGN', 'THD', '2024-12-14 17:00', 100, 'VN0001'),
    ('VN005', 'SGN', 'PQC', '2024-12-17 16:00', 190, 'VN0001'),
    ('VN006', 'SGN', 'PQC', '2024-12-17 2:00', 165, 'VN0001'),
    ('VN007', 'SGN', 'PQC', '2024-12-17 3:30', 140, 'VN0002');

CREATE TABLE bookings (
    booking_id INT PRIMARY KEY AUTO_INCREMENT,
    status VARCHAR(255),
    booking_time TIMESTAMP,
    total_price DECIMAL(10, 2),
    updated_at TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    user_id INT,
    flight_id varchar(255),
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    foreign key (flight_id) references flights(flight_id) on delete cascade
);

CREATE TABLE booking_details (
    booking_detail_id INT PRIMARY KEY AUTO_INCREMENT,
    booking_id INT,
    type varchar(255),
    seat_code varchar(255),
    FOREIGN KEY (booking_id) REFERENCES bookings(booking_id) ON DELETE CASCADE
);

Create table announces (
    announce_id int primary key auto_increment,
    flight_id varchar(255),
    content text,
    created_at timestamp default current_timestamp,
    updated_at timestamp default current_timestamp,
    foreign key (flight_id) references flights(flight_id)
);

INSERT INTO announces (flight_id, content) 
VALUES 
    ('VN001', 'Flight VN001 is ready for boarding.'),
    ('VN002', 'Flight VN002 is delayed due to weather conditions.'),
    ('VN003', 'Flight VN003 will depart in 30 minutes.'),
    ('VN004', 'Flight VN004 is now boarding at gate A3.'),
    ('VN005', 'Flight VN005 is delayed due to technical issues.'),
    ('VN006', 'Flight VN006 has been canceled.'),
    ('VN007', 'Flight VN007 is now boarding at gate B2.');