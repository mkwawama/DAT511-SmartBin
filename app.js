var mqtt    = require('mqtt');
var express = require("express");
var bodyParser = require("body-parser");


var app = express();
app.set("view engine", "ejs");
app.use(bodyParser.urlencoded({ extended: true }));
app.use(express.static('public'));

var client  = mqtt.connect("mqtt://localhost",{clientId:"mqttjs01"});
console.log("connected flag  " + client.connected);

var devices = [];  // list of devices publishing messages

//handle incoming messages
client.on('message',function(topic, message, packet){
    var msg= JSON.parse(message);
    console.log(msg);
    var found = false;
    
    if (msg.distance < 14 && msg.distance > 0)
        msg.level=2;     // bin is full
    if (msg.distance < 28 && msg.distance > 14)
        msg.level=1;  // bin is half way
    if (msg.distance  > 28)   // bin is empty
        msg.level=0;
   
    for (i =0; i< devices.length;i++){ // go through the existing list of devices data
     if (msg.id == devices[i].id){
      found = true;
      devices.splice(i,1);  //delete msg if exists in the list
      devices.push(msg); // push the new message
      }
    }
    if (!found)
      devices.push(msg); // add data if the ID is not in the list
    console.log(devices);
});



client.on("connect",function(){	
console.log("connected  "+ client.connected);

})
//handle errors
client.on("error",function(error){
console.log("Can't connect" + error);
process.exit(1)});


//////////////


var topic="smartcity/smartbin";

function subscribe(){
console.log("subscribing ... ");
client.subscribe(topic); //single topic
}


//=====================
// ROUTES
//=====================

// Showing home page
app.get("/", function (req, res) {
	subscribe();
	res.render("home",{message:devices});
});

var port = process.env.PORT || 3000;
app.listen(port, function () {
	console.log("Server Has Started on port "+port);
});

