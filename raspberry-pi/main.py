
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

    # Wait indefinitely for new serial data
    while uartString[0] is not '{':
      uartString = ser.readline() 

    # Parse uartString to json object
    uartJson = json.loads(uartString)
    
    # Insert the current time & date into the json object 
    time_hhmmss = time.strftime('%H:%M:%S')
    date_mmddyyyy = time.strftime('%d/%m/%Y')
    uartJson['time'] = time_hhmmss
    uartJson['date'] = date_mmddyyyy
    
    # update send json in fireBase
    result = requests.put(firebase_url + '/Data.json', data=json.dumps(uartJson))
    print 'Data Update Successful. result: ' + str(result.status_code) + ',' + result.text

  except IOError:
    print('MicroWatch ERROR: Exception in main loop.')