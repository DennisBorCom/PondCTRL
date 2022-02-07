/**
 * Pond[CTRL]™ WiFi module firmware, © Copyright Dennis Bor, all rights reserved. Pond[CTRL]™ is a trademark of Dennis Bor. 
 */

const char MAIN_page[] PROGMEM = R"=====(
<!-- html validated on 2020-03-18 using https://validator.w3.org/ -->
<!DOCTYPE html>
<html lang="en">
<head>
<title>Pond[CTRL] WiFi Configuration</title>
<style>

html, body {   
  font-family: "Helvetica Neue", Helvetica, sans-serif;   
  color: #000000;   
  -webkit-font-smoothing: antialiased;    
  background: #f0f0f0;
}

form {
    margin: 0 auto;
}

input[type=text],
input[type=password] {
    color: #777;
    padding-left: 10px;
    margin: 10px;
    margin-top: 12px;
    margin-left: 18px;
    width: 290px;
    height: 35px;
    border: 1px solid #c7d0d2;
    border-radius: 2px;
    box-shadow: inset 0 1.5px 3px rgba(190, 190, 190, .4), 0 0 0 5px #f5f7f8;   
}

input[type=text]:hover,
input[type=password]:hover {
    border: 1px solid #b6bfc0;
    box-shadow: inset 0 1.5px 3px rgba(190, 190, 190, .7), 0 0 0 5px #f5f7f8;
}

input[type=text]:focus,
input[type=password]:focus {
    border: 1px solid #a8c9e4;
    box-shadow: inset 0 1.5px 3px rgba(190, 190, 190, .4), 0 0 0 5px #e6f2f9;
}

label {
    color: #555;
    display: inline-block;
    margin-left: 18px;
    padding-top: 10px;
    font-size: 14px;
}

#login {
  width: 320px;
}

input[type=submit] {
    margin-left: 18px;
    margin-top: 10px;
    width: 80px;
    height: 30px;
    font-size: 14px;
    font-weight: bold;
    color: #fff;
    background-color: #acd6ef; /*IE fallback*/
    background-image: -webkit-gradient(linear, left top, left bottom, from(#acd6ef), to(#6ec2e8));
    background-image: -moz-linear-gradient(top left 90deg, #acd6ef 0%, #6ec2e8 100%);
    background-image: linear-gradient(90deg, #acd6ef, #6ec2e8);
    border-radius: 30px;
    border: 1px solid #66add6;
    box-shadow: 0 1px 2px rgba(0, 0, 0, .3), inset 0 1px 0 rgba(255, 255, 255, .5);
    cursor: pointer;
}

span.title {
    color: #555;
    display: inline-block;
    margin-left: 18px;
    padding-top: 15px;
    font-size: 25px; 
}

select {
  color: #777;
    padding-left: 10px;
    margin: 10px;
    margin-top: 12px;
    margin-left: 18px;
    width: 303px;
    height: 35px;
    border: 1px solid #c7d0d2;
    border-radius: 2px;
    box-shadow: inset 0 1.5px 3px rgba(190, 190, 190, .4), 0 0 0 5px #f5f7f8;
}

a:link {
  color: red;
  text-decoration: none;
}

a:hover {
  color: red;
  text-decoration: underline;
}

a:visited {
  color: red;
  text-decoration: none;
}

#macaddress {
  font-weight: bold;
}

#message {
  width:320px;
  margin-left: 18px;
  margin-top: 12px;
}

</style>
</head>
<body>

  <div id="login" style="display:none;">
  
    <span class="title">Pond[CTRL] WiFi Configuration</span><br/>
  
    <form onsubmit="return false;">
      
      <label for="username">Username</label><br/>
      <input type="text" id="username" placeholder="Enter username"/><br/>
      <label for="password">Password</label><br/>
      <input type="password" id="password" placeholder="Enter password"/><br/>
      <input type="submit" value="Login" onclick="javascript:doLogin();">

    </form>
  
  </div>
  
  <div id="networkselection" style="display:none;">
  
    <span class="title">Pond[CTRL] WiFi Configuration</span><br/>
    
    <form onsubmit="return false;">
    
      <label for="ssid">Choose network</label><br/>
      <select name="ssid" id="ssid">
        <option value="" disabled="disabled" selected="selected">Select network</option>
      </select><br/>
      <label for="passphrase">Enter passphrase</label><br/>
      <input type="password" name="passphrase" id="passphrase" placeholder="Enter network passsphrase" maxlength="63"/><br/>    
      <input type="submit" value="Connect" onclick="javascript:setWiFi();"/>  
      <input type="submit" value="Rescan" onclick="javascript:clearSSIDList();httpGet('/listNetworks');"/>
    
    </form>
    
  
  </div>
  
  <div id="done" style="display:none;">
  
    <span class="title">Pond[CTRL] WiFi Configuration</span><br/>
    
    <div id="message">
    
      Pond[CTRL] will now try to connect with your network. If succesful, the open network will disappear. Go to <a href="http://pondctrl.dennisbor.com">the Pond[CTRL] website</a> and register your device to continue. If not succesful, reconnect to the open network and try again.<br/><br/>
      
      To register you need your device serial number: <span id="macaddress"></span>.<br/><br/>
      
      You will be redirected automatically after 30 seconds.
    
    </div>
    
    
  
  </div>
  
  

  <script>
  
    currentURL = '';
    currentAddress = '';
    
    function setWiFi() {
      httpPost(
        '/setWiFi', 
        'ssid=' + document.getElementById('ssid').value + '&passphrase=' + document.getElementById('passphrase').value
      );
    }
    
    function handleResponse(jsonString) {
      var obj = JSON.parse(jsonString);
      if (currentURL.endsWith('/doLogin')) {
        if (obj.status == 'no') {
          alert('Invalid username or password.');
          return;
        } else {
          httpGet('/getMac');
          return;
        }
      }
      if (currentURL.endsWith('/getMac')) {
        if (obj.status == 'ok') {
          document.getElementById('macaddress').innerHTML = obj.macaddress;
        }
        toggleSection('networkselection');
        httpGet('/listNetworks'); 
        return;
      }
      if ((currentURL.endsWith('/listNetworks')) && (obj.status == 'ok')) {
        clearSSIDList();
        for (iterator = 0; iterator < obj.networks.length; iterator ++) {
          var option = document.createElement('option');
          option.value = obj.networks[iterator];
          option.innerHTML = obj.networks[iterator];
          document.getElementById('ssid').appendChild(option);
        }   
        return; 
      }
      if ((currentURL.endsWith('/setWiFi')) && (obj.status == 'ok')) {
        toggleSection('done');
        setTimeout(function() {
          window.location.replace("http://pondctrl.dennisbor.com");
        }, 30000);
      }
    }
  
    function clearSSIDList() {
      for (var iterator = document.getElementById('ssid').options.length - 1; iterator >= 1; iterator--) {
        document.getElementById('ssid').remove(iterator);
      }
      document.getElementById('ssid').value = '';
    }

    function doLogin() {
      if ((document.getElementById('username').value != '') && (document.getElementById('password').value != '')) {
        var username = document.getElementById('username').value;
        var password = document.getElementById('password').value;
        httpPost('/doLogin', 'username=' + username + '&password=' + password); 
      } else {
        toggleSection('login');
        alert('No username or password specified.');
      }
    }
  
    function httpGet(theUrl) {
      currentURL = theUrl;
      if (window.XMLHttpRequest) {
        xhr = new XMLHttpRequest();
      } else {
        xhr = new ActiveXObject('Microsoft.XMLHTTP');
      }
      xhr.timeout = 5000;
      xhr.onreadystatechange = function() {
        if ((xhr.readyState == 4) && (xhr.status == 200)) {
          handleResponse(xhr.responseText);
        } else if (xhr.readyState == 4) {
          alert('Pond[CTRL] not responding.');
        }
      }
      xhr.open('GET', currentAddress + theUrl, true );
      xhr.send();    
    }


    function httpPost(theUrl, postData) {
      currentURL = theUrl;
      if (window.XMLHttpRequest) {
        xhr = new XMLHttpRequest();
      } else {  
        xhr = new ActiveXObject('Microsoft.XMLHTTP');
      }
      xhr.timeout = 5000;
      xhr.onreadystatechange = function() {
        if ((xhr.readyState == 4) && (xhr.status == 200)) {
          handleResponse(xhr.responseText);
        } else if (xhr.readyState == 4) {
          alert('Pond[CTRL] not responding.');
        } 
        
      }
      xhr.open('POST', currentAddress + theUrl, true );
      xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
      xhr.send(postData);
    }

    function toggleSection(elementId) {
      const availableSections = [ 'login', 'networkselection', 'done'];
      for (var iterator = 0; iterator < availableSections.length; iterator ++) {
        if (elementId != availableSections[iterator]) {
          document.getElementById(availableSections[iterator]).style.display = 'none';
        } else {
          document.getElementById(availableSections[iterator]).style.display = 'inline';
        }
      } 
    }
    
    document.addEventListener('DOMContentLoaded', function() {

      currentAddress = window.location.protocol + "//" + window.location.hostname;
      
      toggleSection('login');   
    }, false);


  </script>
  
  
</body>
</html>
)=====";
