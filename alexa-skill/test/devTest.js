var firebaseAdmin = require("firebase-admin");
firebaseAdmin.initializeApp({
  credential: firebaseAdmin.credential.cert("firebase-auth.json"),
  databaseURL: "https://freertos-monitoring.firebaseio.com"
});

var dataRef = firebaseAdmin.database().ref("Data");

dataRef.once('value', function(firebaseData) {
  console.log("Data: "+firebaseData);
});

dataRef.set({
  "date" : "19/12/2016",
    "time" : "00:29:37",
    "temp": 87.46, 
    "light": 8, 
    "x": -88, 
    "y": 75, 
    "sw1": 0, 
    "sw2": 0, 
    "sw3": 0, 
    "sw4": 0, 
    "globalUsed":  1572, 
    "mallocUsed": 24788, 
    "mallocAvail": 0, 
    "systemAvail": 39176
}, function (error){
  if(error){
    console.log("ERROR: Writing to firebase");
  }
});