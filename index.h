static const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style type="text/css">
.button {
  background-color: rgb(15, 170, 10); /* Green */
  border: none;
  color: rgb(241, 233, 233);
  padding: 15px 32px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
}
.button2 {background-color: #f99f9f;} /* Red */ 

*, *::before, *::after {
  box-sizing: border-box;
}

body {
  padding: 0;
  margin: 0;
}

.progress-bar {
  position: relative;
  width: 500px;
  height: 3em;
  background-color: #111;
  border-radius: 1.5em;
  color: white;
}
.progress-bar::before {
  content: attr(data-label);
  display: flex;
  align-items: center;
  position: absolute;
  left: .5em;
  top: .5em;
  bottom: .5em;
  width: calc(var(--width, 0) * 1%);
  min-width: 2rem;
  max-width: calc(100% - 1em);
  background-color: #069;
  border-radius: 1em;
  padding: 1em;
}


.progress-bar1 {
    position: relative;
    width: 500px;
    height: 3em;
    background-color: #111;
    border-radius: 1.5em;
    color: white;
  }
  .progress-bar1::before {
    content: attr(data-label);
    display: flex;
    align-items: center;
    position: absolute;
    left: .5em;
    top: .5em;
    bottom: .5em;
    width: calc(var(--width, 0) * 1%);
    min-width: 2rem;
    max-width: calc(100% - 1em);
    background-color: #069;
    border-radius: 1em;
    padding: 1em;
  }
</style>
<body style="background-color: #cccecf ">
<center>
<div>
<h1>Alexa controled water valve</h1>
  <button class="button" onclick="send('UP_ON')">Valve Upper On</button>
  <button class="button" onclick="send('UP_OFF')">Valve Upper Off</button><BR>
    <h3>
      Percent: <span id="ProgVal2">NA</span>
  </h3>
  <div class="progress-bar1" style="--width: 50" data-label='Progress_Valve_Lower...' ></div>
  <BR><BR>
  <button class="button" onclick="send('LO_ON')">Valve Lower On</button>
  <button class="button" onclick="send('LO_OFF')">Valve Lower Off</button><BR>
    <h3>
      Percent: <span id="ProgVal1">NA</span>
  </h3>
  </div>
  <div class="progress-bar" style="--width: 50" data-label='Progress_Valve_Upper...' ></div><BR>
</div>
 <br>
<div>
<h3>
  Valve Sensor status : <span id="state">NA</span><br><br>
  UP Time : <span id="TimeVal">NA</span>
</h3>
<button class="button button2" onclick="send('STOP_ALL')">STOP ALL</button><BR>
<h3>Last command responce : <span id="cmdRes">NA</span></h3>
<script>
 const progressBar = document.getElementsByClassName('progress-bar')[0];
 const progressBar1 = document.getElementsByClassName('progress-bar1')[0];

function send(Valve_sts) 
{
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
     if (this.readyState == 4 && this.status == 200) {
      document.getElementById("cmdRes").innerHTML =
      this.responseText;
    } 
  };
  xhttp.open("GET", "Set_Valve?state="+Valve_sts, true);
  xhttp.send();
}

setInterval(function() 
{
  getData();
  getTime();
}, 2000); 

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
     if (this.readyState == 4 && this.status == 200) {
      var res = this.responseText.split(",");
      var persent_str = res[4].split(":");
      var persent = parseFloat(persent_str[1])
      progressBar.style.setProperty('--width',persent );
      persent_str = res[5].split(":");
      var persent1 = parseFloat(persent_str[1])
      progressBar1.style.setProperty('--width',persent1 );
      document.getElementById("ProgVal1").innerHTML = persent;
      document.getElementById("ProgVal2").innerHTML = persent1;
      document.getElementById("state").innerHTML = res[0]+res[1]+res[2]+res[3];
    }
    
  };
  xhttp.open("GET", "Status", true);
  xhttp.send();
}

function getTime() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
     if (this.readyState == 4 && this.status == 200) {
      document.getElementById("TimeVal").innerHTML =
      this.responseText;
    }
    
  };
  xhttp.open("GET", "uptime", true);
  xhttp.send();
}
</script>
</center>
</body>
</html>
)=====";
