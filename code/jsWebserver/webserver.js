var http = require('http');
var url = require('url');
var fs = require('fs');

var dgram = require('dgram');
var Engine = require('tingodb')(),
    assert = require('assert');

var db = new Engine.Db('.', {});
var collection = db.collection("usersDb");

// Create socket
var server = dgram.createSocket('udp4');
const querystring = require('querystring');

var HOST = '192.168.1.131';
var WEBPORT = 3000;
var UDPPORT = 8080;

// ===============================================
// ============= WEBSERVER =======================
// ===============================================

console.log('Initializing Webserver on port: ' + WEBPORT);

http.createServer(function (req, res) {
  var q = url.parse(req.url, true);
  var filename = "static/" + q.pathname;


  // '/data' endpoint used to retrieve real time db content
  // example: http://localhost:3000/data?dbcall={%22ID%22:1}
  if (q.pathname == "/data") {
    res.setHeader('Content-Type', 'application/json');    

    if (q.query.dbcall) {
      //console.log('executing ' + q.query.dbcall + ' db query\n');
      var jsonObject = JSON.parse(q.query.dbcall);

      db = new Engine.Db('.', {});
      collection = db.collection("usersDb");

      dbCall = []
      collection.find(jsonObject, {}).toArray(function(err, result){
        if (err) {return res.end("error pulling from db");}
        else {return res.end(JSON.stringify(result));}
        db.close();
      });
    }

    else {
      return res.end("you must provide a query.");
    }
  }

  // '/logout' endpoint used to log out user
  // example: http://localhost:3000/logout?user=chase
  else if (q.pathname == "/logout") {
    res.setHeader('Content-Type', 'application/json');    

    if (q.query.user) {
      // user told us their name in the GET request, ex: http://host:8000/?name=Tom
      console.log('logging out user ' + q.query.user + '\n');
      sendToEsp("Denied", q.query.user);
      return res.end(q.query.user + " logged out.");
    }
    else {
      return res.end("you must provide a user.");
    }
  }
  // http://localhost:3000/loggedin
  else if (q.pathname == "/loggedin") {
      res.setHeader('Content-Type', 'application/json');  
      //console.log("loggedin endpoint hit, returned: ", LoggedIn);
      return res.end(JSON.stringify(Array.from(LoggedIn).reverse()));
  }

  // otherwise, look for html page to return to web browser
  else{ 
    fs.readFile(filename, function(err, data) {
      if (err) {
        fs.readFile("static/404.html", function(err, data) {
          res.writeHead(200, {'Content-Type': 'text/html'});
          res.write(data);
          return res.end();
        });
      }
      else {
        res.writeHead(200, {'Content-Type': 'text/html'});
        res.write(data);
        return res.end();
      }
  });
}
}).listen(WEBPORT);
// ============= END WEBSERVER ===================



// ===============================================
// ================= KEY FOB =====================
// ===============================================

ESPCLIENTS = { chase : {code : 1234, ip : "", port : ""},
               megan : {code : 1235, ip : "", port : ""},
               leila : {code : 1234, ip : "", port : ""} }

const userCodeSet = new Set([ESPCLIENTS.chase.code, ESPCLIENTS.megan.code, ESPCLIENTS.leila.code]);
codeToUsers = {1234 :"chase", 1235 : "megan", 3333 : "leila"}
const LoggedIn = new Set();


// Create server for key fob to connect to
server.on('listening', function () {
    var address = server.address();
    console.log('Initializing UDP Server on ' + address.address + ":" + address.port + "\n\n");
});

// on key fob connection save its ip:port
server.on('message', function (message, remote) {
    try {
      ESPCLIENTS[message].port = remote.port
      ESPCLIENTS[message].ip = remote.address

      console.log(message + '\'s key fob connected and initialized esp @: ' + remote.address + ':' + remote.port)
    }
    catch {
      console.log("Unexpected non init message from ESP: "+ message)
    }
});

// function to send data back to key fob after initial connection
function sendToEsp(serialData, name) {

    try {


    var port = ESPCLIENTS[name].port
    var host = ESPCLIENTS[name].ip



    // error checking
    if (port == "" || host == "") {
      console.log("user key fob is not connected to udp server")
      throw "err"
    }

    // login status checking
    else if (serialData == "Authenticated") {
      LoggedIn.add(name)
    }
    else if (serialData == "Denied" && LoggedIn.has(name)) {
      LoggedIn.delete(name)
    }

    //send message to esp
    server.send(serialData, port, host,function(error){
        if(error) {
          console.log('ERROR: Sending ' + serialData + ' Signal to ESP.');
        }
        else {
          console.log('- Sending ' + serialData + ' to ESP: ' + port + ':' + host);
        }
      });
 

    }
    catch {
      console.log("no one with the name: "+ name + " connected to udp server")
    }
}
server.bind(UDPPORT, HOST);
// ================= END KEY FOB =================



// ===============================================
// ============= DATABASE CALLS ==================
// ===============================================


// jsonObject = [{key : value, key : value, ...}]
function dbAdd(jsonObject) {
  if (lastClick >= (Date.now() - delay))
    return;
  lastClick = Date.now();

  db = new Engine.Db('.', {});
  collection = db.collection("usersDb");

  collection.insert([jsonObject], {w:1}, function(err, result) {
    //assert.equal(null, err);
    if (err) {console.log("dbadd error")}
      db.close();
  });
}

function dbGet(jsonObject) {
  db = new Engine.Db('.', {});
  collection = db.collection("usersDb");
  
  collection.find(jsonObject, {}).toArray(function(err, result){
    dbCall = result;
    console.log(result);
    console.log(result.length);
    db.close();
    });
}
var hi = [{Time : 124, ID : 2, Smoke : 124, Temp : 124}]



// ========== END DATABASE CALLS =================



// ===============================================
// ============= SERIAL CONNECTION ===============
// ===============================================
const SerialPort = require('serialport');
const Readline = SerialPort.parsers.Readline;
const port = new SerialPort('/dev/ttyUSB0', {baudRate: 115200,
parser: Readline});

function tryParseJson (str) {
    try {
        JSON.parse(str);
    } catch (e) {
        return false;
    }
    return JSON.parse(str);
}

// {fob_ID, hub_ID, code}
port.on('open', function () {
  console.log('Initializing Serial port.');
  
  port.on('data', function (data) {
    const sensorData = tryParseJson(data);
    console.log("hub sent data: ", sensorData.fob_ID, sensorData.hub_ID, sensorData.code);

    checkUser(sensorData.fob_ID, sensorData.hub_ID, sensorData.code)
  });
});

var  lastClick = 0;
var  delay = 5000;

function checkUser(fobID, hubID, Code){
  var today = new Date();
  var date = today.getFullYear()+'-'+(today.getMonth()+1)+'-'+today.getDate();
  var time = today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds();

  if (userCodeSet.has(parseInt(Code))) {
    sendToEsp("Authenticated", codeToUsers[Code]);
    // make call to database for user
    dbAdd({ person: codeToUsers[Code], "fob_ID" : fobID, "hub_ID" : hubID, "code" : Code, "date" : date, "time" : time, location : "ec444 room"})
    
  }
  else {
    sendToEsp("Denied", codeToUsers[Code]);
  }
}

//============= END SERIAL CONNECTION ===============




      // // //recursively poll the result variable until db query finished. return result to endpoint.
      // function poll() {
      //   if (dbCall.length != 0) {
      //     console.log("returning")
      //     return res.end(JSON.stringify(dbCall));
      //   }
      //   else{
      //     console.log("poll")
      //     setTimeout(poll, 1000);
      //   }
      // } setTimeout(poll, 1000);
