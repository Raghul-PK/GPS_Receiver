import time
import pyrebase
import folium

config = {
  "apiKey": "AIzaSyAlgstOOV3bVce6LcOZA1vvN-Us24yucIo",
  "authDomain": "gps-tracker2-13e30.firebaseapp.com",
  "databaseURL": "https://gps-tracker2-13e30-default-rtdb.firebaseio.com",
  "projectId": "gps-tracker2-13e30",
  "storageBucket": "gps-tracker2-13e30.appspot.com",
  "messagingSenderId": "933803274902",
  "appId": "1:933803274902:web:b0374e6da1957bb5e188b7"
};

firebase = pyrebase.initialize_app(config)
db = firebase.database()

# enter today's date in GMT
date = "060124"

def convertGMTtoIST(GMT_time):
  GMT_hrs = int(GMT_time[:2])
  GMT_mins = int(GMT_time[2:4])
  GMT_secs = int(GMT_time[4:])

  IST_hrs = GMT_hrs + 5
  IST_mins = GMT_mins + 30;
  if (IST_mins > 60):
    IST_hrs = IST_hrs + 1
    IST_mins = IST_mins - 60

  return str(IST_hrs) + ":" + str(IST_mins) + ":" + str(GMT_secs)

def displayLocsOnMap():
  date_dict = db.child(date).get()
  time_entry = list(date_dict.val())
  loc_list = []
  for time_stamp in time_entry:
    GPS_data = db.child(date).child(time_stamp).get()
    [lat, long] = getLatLong(GPS_data, time_stamp)
    if not (lat == 0 or long == 0):
      IST_timestamp = convertGMTtoIST(time_stamp)
      loc_list.append([IST_timestamp, lat, long])
  print(loc_list)

  map = folium.Map(location=[loc_list[0][1], loc_list[0][2]],zoom_start=30)
  # Build the map
  for loc in loc_list:
    timestamp = loc[0]
    lat = loc[1]
    long = loc[2]
    folium.Marker(location=[lat, long], popup=timestamp, icon=folium.Icon(color='green')).add_to(map)
  map.save("map_" + date + ".html")

def displayLatestLocOnMap():
  date_dict = db.child(date).get()
  time_entry = list(date_dict.val())[-1]
  GPS_data = db.child(date).child(time_entry).get()
  [lat, long] = getLatLong(GPS_data)
  map = folium.Map(location=[lat, long], zoom_start=15)
  map.add_child(folium.Marker(location=[lat, long], popup='Your location', icon=folium.Icon(color='green')))
  map.save("map.html")

def getLatLong(gps_data, time_stamp):
  nmea_latitude = gps_data.val()["Lat"]
  nmea_longitude = gps_data.val()["Long"]
  print("Time : " + time_stamp + " --> " + "NMEA Lat/Long:", nmea_latitude, " ,", nmea_longitude)

  if (nmea_latitude=="0" or nmea_longitude=="0"):
    return [0,0];

  # Conversion of Lat/Long (Degrees and Decimal minutes) --> Lat/Long (Decimal degrees)
  degrees_latitude = int(nmea_latitude[:2])
  minutes_latitude = float(nmea_latitude[2:])
  decimal_latitude = degrees_latitude + (minutes_latitude / 60)
  degrees_longitude = int(nmea_longitude[:3])
  minutes_longitude = float(nmea_longitude[3:])
  decimal_longitude = degrees_longitude + (minutes_longitude / 60)
  print("Decimal Lat/Long:", decimal_latitude, " ,", decimal_longitude)
  print()

  return [decimal_latitude, decimal_longitude]


displayLocsOnMap()
print("Map over")
