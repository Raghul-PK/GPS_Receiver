from flask import Flask
import requests, json
app = Flask(__name__)

from datetime import datetime

# Get last known location of a specific vehicle
# Identified by its GPS_id
@app.route("/<string:gps_id>/latestLoc/", methods=['GET'])
def getLatestLocation(gps_id):
    url = "https://gps-tracker2-13e30-default-rtdb.firebaseio.com/" + gps_id + ".json?auth=zhw6Zkn3OQXjojhr8dDPn3OTJFdzhNk27c828puQ"
    req = requests.get(url)
    json_data = json.loads(req.content) # This is a dictionary
    json_dates = json_data.keys()
    valid_dates = [date for date in json_dates if len(date) == 6 and date.isdigit()]
    print(valid_dates)

    # Get the latest date
    formatted_dates = [datetime.strptime(date, "%d%m%y") for date in valid_dates]
    sorted_dates = sorted(formatted_dates)
    sorted_dates_str = [date.strftime("%d%m%y") for date in sorted_dates]
    print(sorted_dates_str)

    # Go to the last date for that particular gps_id
    latestDate = list(sorted_dates_str)[-1]
    latestDate_data = json_data[latestDate]
    print(latestDate_data)

    # Go to the last timestamp for that particular date
    latestTime = list(latestDate_data)[105]
    latestTime_data = latestDate_data[latestTime]
    latestTime_data["Date"] = latestDate
    latestTime_data["Time"] = latestTime

    return latestTime_data

# Get journey of a specific vehicle on a specific date
# Identified by its GPS_id
@app.route("/<string:gps_id>/journey/<string:date>/", methods=['GET'])
def getDateJourney(gps_id, date):
    url = "https://gps-tracker2-13e30-default-rtdb.firebaseio.com/" + gps_id + ".json?auth=zhw6Zkn3OQXjojhr8dDPn3OTJFdzhNk27c828puQ"
    req = requests.get(url)
    json_data = json.loads(req.content)  # This is a dictionary
    print(json_data)

    if date in json_data:
        # Go to the specified date
        Date_data = json_data[date]
        print(Date_data)
        print(type(Date_data))
        return Date_data
    else:
        return ("Date doesn't exist")

app.run()
