
import serial
import time
import requests
import json
firebase_url = 'https://freertos-monitoring.firebaseio.com'
#Connect to Serial Port for communication
ser = serial.Serial("/dev/ttyUSB0", 9600, timeout=None)

print "Starting MicroWatch fireBase transmission!\n\nWaiting for UART Data transmission..."

while 1:
  try:

    # Clear uartString for fresh data
    uartString = "."
    uartJson = {}
    # Wait indefinitely for new serial data
    while uartString[0] is not '{':
      uartString = ser.readline(None)

    # Parse uartString to json object
    uartJson = json.loads(uartString)
    
    # Insert the current time & date into the json object 
    time_hhmmss = time.strftime('%H:%M:%S')
    date_mmddyyyy = time.strftime('%m/%d/%Y')
    uartJson['time'] = time_hhmmss
    uartJson['date'] = date_mmddyyyy
    
    # update send json in fireBase
    result = requests.put(firebase_url + '/Data.json', data=json.dumps(uartJson))
    if result.status_code is 200:
      print 'Data Update Successful. result: ' + str(result.status_code) + ',' + result.text
    else:
      print 'fireBase Data send failed with' + str(result.status_code) + ',' + result.text

  except IOError:
    print('MicroWatch IOError: Serial data could not be read!\n')

  except ValueError:
    print('MicroWatch ValueError: Serial data not parsed to json!')
    print('  Likely, bad serial data was read. retrying on next data recieved...\n')