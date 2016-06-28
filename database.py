#!/usr/bin/python3
from config import app

def _enter_data(title):
	#Get ID from user
	return input(title)

def get_truck(tag_id, cursor, conn):
	try:
		#Get record corresponding to the entered ID
		sql = "SELECT trucks.id, truck_configurations.gross_mass FROM trucks JOIN truck_configurations\
				ON trucks.configuration_id = truck_configurations.id WHERE trucks.tag_id = {}".format(str(tag_id))

		cursor.execute(sql)

		return cursor.fetchone()

	except conn.DatabaseError as e:
		return e

def get_station_id(station_number, cursor, conn):
	try:
		#Get record corresponding to the entered ID
		sql = "SELECT weighbridge_stations.id FROM weighbridge_stations \
			   WHERE weighbridge_stations.station_number = '{}'".format(station_number)

		cursor.execute(sql)

		result = cursor.fetchone()

		return result[0]

	except conn.DatabaseError as e:
		return e

def insert_truck_data(data, cursor, conn):

	try:
		sql = "INSERT INTO truck_measured_data (truck_id, gross_mass, remarks, measured_at, station_id ) \
		       VALUES('{0}', '{1}', '{2}', '{3}', '{4}')".format(*data)

		cursor.execute(sql);
		conn.commit()

		conn.close()
		print('data saved!')

	except conn.DatabaseError as e:
		print('Error {}'.format(e))
