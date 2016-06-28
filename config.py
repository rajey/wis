#!/usr/bin/python3
import pymysql
import serial
import time

app = dict(
    STATION_NUMBER = '2063XBUVKG'
)

def db_connection():
    try:
        return pymysql.connect(
		db = "vocms",
		user = "root",
		password = "woodnymph",
		host = "localhost"
		)
    except:
        print('Failed to connect to database')
        exit()

def serial_connection():
    ser = serial.Serial()
    ser.port = '/dev/ttyUSB0'
    ser.baudrate = 9600
    ser.bytesize = serial.EIGHTBITS

    # attempt serial connection to arduino
    print("Connecting to arduino.......")
    while True:
        try:
            ser.open()
            print("Connection with arduino established!")
            break
        except:
            continue

    if ser.isOpen():
        try:
            ser.flushInput() #flush input buffer, discarding all its contents
            ser.flushOutput()#flush output buffer, aborting current output
            return ser
        except serial.SerialException as e:
            print(e)
            exit()
