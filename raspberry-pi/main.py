
import serial
import time
import requests
import json
firebase_url = 'https://freertos-monitoring.firebaseio.com'
#Connect to Serial Port for communication

ser = serial.Serial("/dev/ttyUSB0", 9600, timeout=1000)
uartString = ""

while not uartString:
  uartString = ser.readline()

#Setup a loop to send Temperature values at fixed intervals
#in seconds
fixed_interval = 0.5
while 1:
  try:
    #temperature value obtained from Arduino + LM35 Temp Sensor          
    #temperature_c = ser.readline().strip()

    # uartString = "{\"temp\": 87.01, \"light\": 8 }"
    uartString = ser.readline()
    # print uartString
    jsonParse = json.loads(uartString)
    
    temperature_c = jsonParse['temp']
    #current time and date

    
    time_hhmmss = time.strftime('%H:%M:%S')
    date_mmddyyyy = time.strftime('%d/%m/%Y')
    jsonParse['time'] = time_hhmmss
    jsonParse['date'] = date_mmddyyyy
    print jsonParse
    
    #current location name
    # print temperature_c + ',' + time_hhmmss + ',' + date_mmddyyyy + ',' + temperature_location
    
    #insert record
    data = {'date':date_mmddyyyy,'time':time_hhmmss,'temperature':temperature_c, 'cpu':12}
    # result = requests.post(firebase_url + '/' + temperature_location + '/temperature.json', data=json.dumps(data))
    result = requests.put(firebase_url + '/Data.json', data=json.dumps(jsonParse))
    
    print 'Record inserted. Result Code = ' + str(result.status_code) + ',' + result.text
    time.sleep(fixed_interval)
  except IOError:
    print('Error! Something went wrong.')
  time.sleep(fixed_interval)