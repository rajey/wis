#!/usr/bin/python3
import datetime
import time
from arduino_serial import *
from database import *
from config import *

def main():
    while True:
        #Get database connection
        db = db_connection()
        cursor = db.cursor()

        #serial connection
        ser = serial_connection()

        #get tagID from arduino
        tagID = get_tag_id(ser)

        #get data from database
        truck = get_truck(tagID, cursor, db)

        if truck:
            #send validation status based on the result from the database
            status = '1'
            send_validation_status(status, ser)

            # get measured gross mass
            measured_gvm = get_measured_gvm(ser)

            # get overload status
            overload_status = _check_for_overloading(float(truck[1]),measured_gvm)

            # send overload status to arduino
            send_overload_status(overload_status, ser)

            # compile data to be saved
            data = []
            data.append(truck[0])
            data.append(measured_gvm)
            data.append(_get_overload_remark(overload_status))
            data.append('{:%Y-%m-%d %H:%M:%S}'.format(datetime.datetime.now()))
            data.append(get_station_id(app['STATION_NUMBER'], cursor, db))

            #insert data into the database
            insert_truck_data(data, cursor, db)

        else:
            status = '0'
            send_validation_status(status, ser)
        #close serial port
        print("Closing connection....")
        ser.close()
        print("Connection closed")
        time.sleep(2)


def _check_for_overloading(orig_gvm, gvm):
    # calculate overloaded amount
    amount = gvm - orig_gvm
    # compute percentage overloaded
    percent = round((amount/orig_gvm)*100, 2)

    if percent > 5.00:
        return '1'
    else:
        return '0'

def _get_overload_remark(status):
    if status == 1:
        return 'overloaded'
    else:
        return 'within limit'



if __name__ == "__main__": main()
