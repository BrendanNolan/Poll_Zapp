import csv
import os
from random import randrange
import sqlite3


path_to_pictures = os.getenv("POLL_ZAPP") + "\\Pictures\\"

primary_database_path = os.getenv("POLL_ZAPP") + "\\databases\\primary_database.db"
primary_database = open(primary_database_path, 'w')
primary_database.close()

db = sqlite3.connect(primary_database_path)
cursor = db.cursor()

cursor.execute('''
    CREATE TABLE politicians 
        (
        id INTEGER PRIMARY KEY AUTOINCREMENT, 
        constituency_id INTEGER, 
        image_url TEXT, 
        name TEXT, 
        elected INTEGER, 
        candidate INTEGER, 
        party_name TEXT, 
        party_rgb_red TEXT, 
        party_rgb_green TEXT, 
        party_rgb_blue TEXT
        )
''')
db.commit()

cursor.execute('''
    CREATE TABLE constituencies  
        (
        id INTEGER PRIMARY KEY AUTOINCREMENT, 
        name TEXT, 
        latitude INTEGER, 
        longitude INTEGER
        )
''')
db.commit()

cursor.execute('''
    CREATE TABLE poll_results  
        (
        constituency_id INTEGER,
        source TEXT,
        date_time DATETIME,
        politician_name TEXT,
        poll_value INT
        )
''')
db.commit()

for i in range(3):
    cursor.execute('''
        INSERT INTO constituencies 
            (name, latitude, longitude) 
            VALUES  
            (?, ?, ?)
    ''', ("Wexford", i * 40, i * 40))
    db.commit()
    constituency_id = cursor.lastrowid

    for j in range(3):
        cursor.execute('''
            INSERT INTO poll_results
            (constituency_id, source, date_time, politician_name, poll_value)
            VALUES
            (?, ?, ?, ?, ?)
        ''', (constituency_id, "NYT" + str(i), 20200201 + i, "Politician" + str(i), i * 40))
        db.commit()

    cursor.execute('''
        INSERT INTO politicians 
                (
                constituency_id, image_url, name, elected, candidate, 
                party_name, party_rgb_red, party_rgb_green, party_rgb_blue
                ) 
                VALUES 
                (?, ?, ?, ?, ?, ?, ?, ?, ?)            
    ''', (constituency_id, path_to_pictures + "pic" + str(i + randrange(3)) + ".jpg", "Brendan", 1, i % 2, "Fianna Fail", 0, 200, 0))
    db.commit()

    cursor.execute('''
        INSERT INTO politicians 
                (
                constituency_id, image_url, name, elected, candidate, 
                party_name, party_rgb_red, party_rgb_green, party_rgb_blue
                ) 
                VALUES 
                (?, ?, ?, ?, ?, ?, ?, ?, ?)            
    ''', (constituency_id, path_to_pictures + "pic" + str(i + randrange(3)) + ".jpg", "Rory", 1, i % 2, "Fine gael", 0, 0, 200))
    db.commit()

    cursor.execute('''
        INSERT INTO politicians 
                (
                constituency_id, image_url, name, elected, candidate, 
                party_name, party_rgb_red, party_rgb_green, party_rgb_blue
                ) 
                VALUES 
                (?, ?, ?, ?, ?, ?, ?, ?, ?)            
    ''', (constituency_id, path_to_pictures + "pic" + str(i + randrange(3)) + ".jpg", "David", 0, i % 2, "Labour", 200, 0, 0))
    db.commit()

    cursor.execute('''
        INSERT INTO politicians 
                (
                constituency_id, image_url, name, elected, candidate, 
                party_name, party_rgb_red, party_rgb_green, party_rgb_blue
                ) 
                VALUES 
                (?, ?, ?, ?, ?, ?, ?, ?, ?)            
    ''', (constituency_id, path_to_pictures + "pic" + str(i + randrange(3)) + ".jpg", "Kate", 1, i % 2, "Labour", 200, 0, 0))
    db.commit()
