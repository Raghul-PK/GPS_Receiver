from flask import Flask
import requests, json
app = Flask(__name__)

# Get last known location of a specific vehicle
# Identified by its GPS_id
@app.route("/<string:gps_id>/latestLoc/", methods=['GET'])
def getLatestLocation(gps_id):
    gps_id = "" # For time being, as we don't have gps-id still in db
    url = "https://gps-tracker2-13e30-default-rtdb.firebaseio.com/" + gps_id + ".json?auth=zhw6Zkn3OQXjojhr8dDPn3OTJFdzhNk27c828puQ"
    req = requests.get(url)
    json_data = json.loads(req.content) # This is a dictionary

    # Go to the last date for that particular gps_id
    latestDate = list(json_data)[-1]
    latestDate_data = json_data[latestDate]
    print(type(latestDate_data))

    # Go to the last timestamp for that particular date
    latestTime = list(latestDate_data)[-1]
    latestTime_data = latestDate_data[latestTime]

    return [latestTime, latestTime_data]

    # # Retrieve last key-value pair in dict
    # # Originally, {'Lat': '0', 'Long': '0', 'SNR': '0'} will not be saved in final version
    # latest_time = list(json_data)[-1]
    # latest_loc = json_data[latest_time]
    # print("Last tracked time : " + str(latest_time))
    # print("Last recorded location data : " + str(latest_loc))
    # return str(latest_time) + str(latest_loc)

# Get journey of a specific vehicle on a specific date
# Identified by its GPS_id
@app.route("/<string:gps_id>/journey/<string:date>/", methods=['GET'])
def getDateJourney(gps_id, date):
    gps_id = ""  # For time being, as we don't have gps-id still in db
    url = "https://gps-tracker2-13e30-default-rtdb.firebaseio.com/" + gps_id + (date + "/") + ".json?auth=zhw6Zkn3OQXjojhr8dDPn3OTJFdzhNk27c828puQ"
    req = requests.get(url)
    json_data = json.loads(req.content)  # This is a dictionary

    return json_data

app.run()
