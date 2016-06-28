#!/usr/bin/python3
import serial
import time
import struct

def _get_serial_data(ser, byte):
    try:
        print('waiting for data from arduino..')
        if (byte == 4):
            return ser.readline(4)
        return ser.readline()
    except:
        return 'Failed to get data from arduino'

def _write_serial_data(data, ser):
    try:
        print('sending data to arduino...');
        time.sleep(1)
        ser.write(bytes(data,'utf-8'))
        print('data sent')
    except:
        print('Failed to send data to arduino')

def get_tag_id(ser):
    id = _get_serial_data(ser, 1).splitlines()
    print('tag id recieved')
    return str(id[0])[1:]

def get_measured_gvm(ser):
    gvm = struct.unpack('f', _get_serial_data(ser, 4))
    print('gross_mass recieved')
    return round(gvm[0], 2)

def send_validation_status(status, ser):
    _write_serial_data(status, ser)

def send_overload_status(status, ser):
    _write_serial_data(status, ser)
