#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <time.h>

#include <DHT.h>
#define SOIL_SENSOR_PIN A0
#define DHTTYPE DHT22
#define DHTPIN 2

DHT dht(DHTPIN, DHTTYPE);

// Wifi info
const char* ssid = "FPT Telecom";
const char* password = "88888888";

// Webserver, port 80
ESP8266WebServer server(80);

WiFiClient wifiClient;

const char* html = R"html(
<!DOCTYPE html>
<html lang="en" style="height: 100%">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Sensors Dashboard</title>
  <script type="text/javascript" src="https://fastly.jsdelivr.net/npm/echarts@5.5.1/dist/echarts.min.js"></script>
  <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.3/font/bootstrap-icons.min.css">
  <style>
    @import url('https://fonts.googleapis.com/css2?family=Nunito:ital,wght@0,200..1000;1,200..1000&display=swap');
    body {
      margin: 0;
      font-family: "Nunito", sans-serif;
      font-optical-sizing: auto;
      font-weight: 500;
      font-style: normal;
      font-size: 14px;
    }
    
    .d-flex {
      display: flex;
      gap: 16px;
    }
    
    .justify-content-between {
      justify-content: space-between;
    }
    
    .align-items-center {
      align-items: center;
    }
    
    h2 {
      text-align: center;
      margin: 16px 0 24px;
      font-size: 24px;
    }
    
    .text-warning {
      color: #ffc107;
    }
    
    .text-primary {
      color: #0d6efd;
    }
    
    .text-success {
      color: #198754;
    }
    
    .text-danger {
      color: #dc3545;
    }
    
    .btn {
      display: inline-flex;
      align-items: center;
      height: 32px;
      border-radius: 8px;
      border: none;
      padding: 0 12px;
      font-size: 14px;
    }
    
    .btn-danger {
      color: #fff;
      background-color: #dc3545;
    }
    
    .form-input {
      height: 30px;
      border-radius: 8px;
      border: 1px solid #ccc;
      padding: 0 12px;
      font-size: 14px;
      background-color: #fff;
    }
    
    .wapper {
      /* width: 360px; */
      padding: 0 24px 70px;
      margin: 0 auto;
    }
    
    .wapper .btn-switch {
      border: 1px solid #eee;
      background-color: #eeeeee;
      border-radius: 20px;
      padding: 10px 10px 6px 10px;
      margin-top: 16px;
      width: 100%;
    }
    
    .wapper .btn-switch .switch {
      position: relative;
      display: inline-block;
      width: 42px;
      height: 24px;
      margin: 4px;
    }
    
    .wapper .btn-switch .switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }
    
    .wapper .btn-switch .switch .slider {
      position: absolute;
      cursor: pointer;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #ccc;
      transition: .4s;
      border-radius: 34px;
    }
    
    .wapper .btn-switch .switch .slider:before {
      position: absolute;
      content: "";
      height: 18px;
      width: 18px;
      left: 3px;
      bottom: 3px;
      background-color: white;
      transition: .3s;
      border-radius: 50%;
    }
    
    .wapper .btn-switch .switch input:checked+.slider {
      background-color: #d92b2d;
    }
    
    .wapper .btn-switch .switch input:checked+.slider:before {
      transform: translateX(18px);
    }
    
    .wapper .btn-switch .btn-name {
      margin: 4px;
      margin-top: 0;
      font-weight: 700;
    }
    
    .wapper .btn-switch .icon i {
      font-size: 30px;
    }
    
    .auto-info {
      padding: 10px 12px;
      margin-top: 16px;
      border-radius: 20px;
      background-color: #d5e6de;
    }
    
    .auto-info.disable {
      background-color: #eee;
      color: #bbb;
    }
    /*
     * NAV ---------------------------------------------------------------------------------------------------------
     */
    
    nav {
      padding: 8px 16px;
      background-color: #eeeeee;
      position: fixed;
      bottom: 0;
      width: calc(100% - 32px);
    }
    
    nav a {
      font-size: 16px;
      text-decoration: none;
      display: inline-block;
      padding: 8px;
      color: #000;
      font-weight: 700;
      border-radius: 8px;
    }
    /*
     * SIDEBAR -----------------------------------------------------------------------------------------------------
     */
    
    .sidenav {
      height: 100%;
      width: 0;
      position: fixed;
      z-index: 1;
      top: 0;
      right: 0;
      background-color: #fff;
      overflow-x: hidden;
      transition: 0.5s;
      padding-top: 60px;
      box-shadow: 0 12px 28px 0 rgb(0 0 0 / 20%), 0 2px 4px 0 rgb(0 0 0 / 10%), inset 0 0 0 1px rgb(255 255 255 / 50%);
    }
    
    .sidenav a {
      padding: 8px 24px 8px 24px;
      text-decoration: none;
      font-size: 20px;
      color: #000;
      display: block;
      transition: 0.3s;
    }
    
    .sidenav a:hover {
      color: #f1f1f1;
    }
    
    .sidenav .closebtn {
      position: absolute;
      top: 0;
      right: 0;
      font-size: 36px;
    }
    
    @media screen and (max-height: 450px) {
      .sidenav {
        padding-top: 15px;
      }
      .sidenav a {
        font-size: 18px;
      }
    }
    
    .tab-button.active {
      color: #cc4749;
    }
  </style>
</head>

<body>
  <div class="wapper">
    <div id="Tab1" class="tab-content">
      <h2>Monitor System</h2>

      <div id="container" style="height: 380px; margin-bottom: 12px;"></div>

      <div class="d-flex justify-content-between">
        <div class="btn-switch">
          <div class="d-flex justify-content-between">
            <div class="switch">
              <input type="checkbox" id="autoCheckbox" checked onchange='toggleAutoState(this)'>
              <label for="autoCheckbox" class="slider"></label>
            </div>
            <div class="icon" style="margin-right: 2px;">
              <i class="bi bi-circle-half text-danger"></i>
            </div>
          </div>
          <div class="btn-name">AUTO</div>
        </div>
        <div class="btn-switch">
          <div class="d-flex justify-content-between">
            <div class="switch">
              <input type="checkbox" id="lightCheckbox" onchange='toggleLight(this)' >
              <label for="lightCheckbox" class="slider"></label>
            </div>
            <div class="icon">
              <i class="bi bi-brightness-high-fill text-warning"></i>
            </div>
          </div>
          <div class="btn-name">LIGHT</div>
        </div>
      </div>

      <div class="d-flex justify-content-between">
        <div class="btn-switch">
          <div class="d-flex justify-content-between">
            <div class="switch">
              <input type="checkbox" id="fanCheckbox" onchange='toggleFan(this)'>
              <label for="fanCheckbox" class="slider"></label>
            </div>
            <div class="icon" style="margin-right: 2px;">
              <i class="bi bi-fan text-success"></i>
            </div>
          </div>
          <div class="btn-name">FAN</div>
        </div>
        <div class="btn-switch">
          <div class="d-flex justify-content-between">
            <div class="switch">
              <input type="checkbox" id="wateringCheckbox" onchange='toggleWater(this)'>
              <label for="wateringCheckbox" class="slider"></label>
            </div>
            <div class="icon">
              <i class="bi bi-droplet-half text-primary"></i>
            </div>
          </div>
          <div class="btn-name">WATERING</div>
        </div>
      </div>

      <div class="auto-info">
        <table>
          <tr>
            <td>LIGHT ON</td>
            <td>18h - 04h</td>
          </tr>
          <tr>
            <td style="vertical-align: top;">FAN ON</td>
            <td>
              <p style="margin-top: 0; margin-bottom: 4px;">Temperature &nbsp;&gt; <span id="maxTemperatureValue">0</span>°C</p>
              <p style="margin-top: 0; margin-bottom: 4px;">Air Humidity &nbsp;&gt; <span id="maxAirHumidityValue">0</span>%</p>
            </td>
          </tr>
          <tr>
            <td style="vertical-align: top; padding-right: 8px;">WATERING ON</td>
            <td>
              <p style="margin-top: 0; margin-bottom: 0;">Soil Humidity &gt; <span id="maxSoilHumidityValue">0</span>%</p>
            </td>
          </tr>
        </table>
      </div>

    </div>

    <div id="Tab2" class="tab-content">
      <h2>Setting</h2>
      <h2>Set Temperature Threshold</h2>
      <form id="tempForm">
        <label for="minTemperature">Min Temperature:</label>
        <input type="number" id="minTemperature" name="minTemperature" required>
        <br>
        <label for="maxTemperature">Max Temperature:</label>
        <input type="number" id="maxTemperature" name="maxTemperature" required>
        <br>
        <label for="deviceName">Device Name:</label>
        <input type="text" id="deviceName" name="deviceName" required>
        <br>
        <input type="submit" value="Set">
      </form>
    </div>

    <div id="Tab3" class="tab-content">
      <h2>Report chart</h2>

      <div style="text-align: center; margin-bottom: 20px;">
        <form id="dateSensorForm">
          <label for="date">Select Date:</label>
          <input class="form-input" type="date" id="date-line-chart" name="date-line-chart" required style="margin-right: 4px;">
          <button class="btn btn-danger" type="submit">View</button>
        </form>
      </div>
      <div id="chartReport" style="height: 280px"></div>

      <div style="text-align: center; margin-top: 30px; margin-bottom: 20px;">
        <form id="monthSensorForm">
          <label for="month">Select Month:</label>
          <input class="form-input" type="month" id="month-line-chart" name="month-line-chart" required style="margin-right: 4px;">
          <button class="btn btn-danger" type="submit">View</button>
        </form>
      </div>
      <div id="chartMonthReport" style="height: 280px"></div>
    </div>
  </div>

  <nav class="d-flex justify-content-between align-items-center">
    <div>
      <a href="javascript:void(0)" class="tab-button active" onclick="openTab(event, 'Tab1')">HOME</a>
    </div>
    <div>
      <a href="javascript:void(0)" class="tab-button" onclick="openTab(event, 'Tab2')">SETTING</a>
    </div>
    <div>
      <a href="javascript:void(0)" class="tab-button" onclick="openTab(event, 'Tab3')">REPORT</a>
    </div>
    <div>
      <a href="javascript:void(0)" onclick="openNav()">MENU</a>
    </div>
  </nav>

  <div id="mySidenav" class="sidenav">
    <a href="javascript:void(0)" class="closebtn" onclick="closeNav()">&times;</a>
    <a href="#">About</a>
    <a href="#">Services</a>
    <a href="#">Clients</a>
    <a href="#">Contact</a>
  </div>

  <!-- VARIABLE -->
  <script type="text/javascript">
    var lightState = false;
    var fanState = false;
    var waterState = false;
  </script>

  <!-- Get init data setting -->
  <script type="text/javascript">
    function initDataSetting() {
      const xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById('maxTemperatureValue').innerText = this.responseText.split('-')[1];
          document.getElementById('maxAirHumidityValue').innerText = this.responseText.split('-')[3];
          document.getElementById('maxSoilHumidityValue').innerText = this.responseText.split('-')[5];

          myChart.setOption({
            series: [{
              data: gaugeData,
              pointer: {
                show: false
              }
            }]
          });
        }
      };
      xhr.open('GET', '/init/setting/value', true);
      xhr.send();
    }
    initDataSetting();
  </script>

  <!-- Set checkbox state - Func -->
  <script type="text/javascript">
    function setCheckboxStatus(checkboxId, status) {
      const checkbox = document.getElementById(checkboxId);
      if (status === '1') {
        checkbox.checked = true;
      } else {
        checkbox.checked = false;
      }
    }
  </script>

  <!-- Open/hide nav - Func -->
  <script type="text/javascript">
    function openNav() {
      document.getElementById("mySidenav").style.width = "250px";
    }

    function closeNav() {
      document.getElementById("mySidenav").style.width = "0";
    }
  </script>

  <!-- Tab - Func -->
  <script type="text/javascript">
    function openTab(evt, tabName) {

      let i, tabcontent, tabbuttons;

      tabcontent = document.getElementsByClassName("tab-content");
      for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
      }

      tabbuttons = document.getElementsByClassName("tab-button");
      for (i = 0; i < tabbuttons.length; i++) {
        tabbuttons[i].className = tabbuttons[i].className.replace(" active", "");
      }

      document.getElementById(tabName).style.display = "block";
      evt.currentTarget.className += " active";
    }

    document.addEventListener('DOMContentLoaded', function() {
      document.querySelector('.tab-button').click();
    });
  </script>

  <!-- Gauge chart -->
  <script type="text/javascript">
    var dom = document.getElementById('container');

    var myChart = echarts.init(dom, null, {
      renderer: 'canvas',
      useDirtyRect: false
    });

    const gaugeData = [{
      value: 0,
      name: 'Air humidity',
      title: {
        offsetCenter: ['0%', '-43%']
      },
      detail: {
        valueAnimation: true,
        offsetCenter: ['0%', '-28%'],
        formatter: '{value}%'
      },
      itemStyle: {
        borderWidth: 1,
        //borderColor: '#464646',
        color: '#0d6efd'
      }
    }, {
      value: 0,
      name: 'Soil humidity',
      title: {
        offsetCenter: ['0%', '-10%']
      },
      detail: {
        valueAnimation: true,
        offsetCenter: ['0%', '5%'],
        formatter: '{value}%'
      },
      itemStyle: {
        borderWidth: 1,
        //borderColor: '#464646',
        color: '#198754'
      }
    }, {
      value: 0,
      name: 'Temperature',
      title: {
        offsetCenter: ['0%', '23%']
      },
      detail: {
        valueAnimation: true,
        offsetCenter: ['0%', '38%'],
        formatter: '{value}°C'
      },
      itemStyle: {
        borderWidth: 1,
        //borderColor: '#464646',
        color: '#dc3545'
      }
    }];

    const option = {
      series: [{
        type: 'gauge',
        startAngle: 90,
        endAngle: -270,
        radius: '100%',
        pointer: {
          show: false
        },
        progress: {
          show: true,
          overlap: false,
          roundCap: true,
          clip: false
        },
        axisLine: {
          lineStyle: {
            width: 70,
            color: [
              [1, '#eee']
            ]
          }
        },
        splitLine: {
          show: false,
          distance: 0,
          length: 10
        },
        axisTick: {
          show: false
        },
        axisLabel: {
          show: false,
          distance: 50
        },
        data: gaugeData,
        title: {
          fontSize: 14
        },
        detail: {
          width: 60,
          height: 18,
          fontSize: 18,
          color: '#fff',
          borderColor: 'inherit',
          backgroundColor: 'inherit',
          borderRadius: 20,
          borderWidth: 1
        }
      }]
    };

    setInterval(function() {
      const xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          gaugeData[0].value = this.responseText.split('-')[0];
          gaugeData[1].value = this.responseText.split('-')[1];
          gaugeData[2].value = this.responseText.split('-')[2];

          lightState = this.responseText.split('-')[3];
          fanState = this.responseText.split('-')[4];
          waterState = this.responseText.split('-')[5];

          setCheckboxStatus('lightCheckbox', lightState);
          setCheckboxStatus('fanCheckbox', fanState);
          setCheckboxStatus('wateringCheckbox', waterState);

          myChart.setOption({
            series: [{
              data: gaugeData,
              pointer: {
                show: false
              }
            }]
          });
        }
      };
      xhr.open('GET', '/monitor', true);
      xhr.send();
    }, 3000);

    if (option && typeof option === 'object') {
      myChart.setOption(option);
    }
    window.addEventListener('resize', myChart.resize);
  </script>

  <!-- Line chart -->
  <script type="text/javascript">
    const today = new Date();
    const c_day = ("0" + today.getDate()).slice(-2);
    const c_month = ("0" + (today.getMonth() + 1)).slice(-2);
    const formattedDate = today.getFullYear() + "-" + c_month + "-" + c_day;
    document.getElementById('date-line-chart').value = formattedDate;

    document.getElementById('dateSensorForm').addEventListener('submit', function(event) {
      event.preventDefault();

      const date = document.getElementById('date-line-chart').value;
      const [year, month, day] = date.split('-');
      getDataSensor(day, month, year);
    });

    function getDataSensor(day, month, year) {
      fetch(`http://localhost:8088/api/temperatures/day?day=${day}&month=${month}&year=${year}`)
        .then(response => response.json())
        .then(data => {
          if (JSON.stringify(data) === '{}') {
            alert("No data");
          } else {
            const keys = Object.keys(data).slice(4);
            const humidityAir = Object.values(data.humidityAir).slice(4);
            const humiditySoil = Object.values(data.humiditySoil).slice(4);
            const temperature = Object.values(data.temperature).slice(4);

            const hourKeys = Array.from({
              length: 24
            }, (_, i) => `${i}h`);

            console.log("Keys:", hourKeys);
            console.log("HumidityAir:", humidityAir);
            console.log("HumiditySoil:", humiditySoil);
            console.log("Temperature:", temperature);

            getChartView(hourKeys, humidityAir, humiditySoil, temperature);
          }
        })
        .catch(error => {
          console.error('Error fetching temperature data:', error);
        });
    }

    function getChartView(hourKeys, humidityAir, humiditySoil, temperature) {
      const domChartReport = document.getElementById('chartReport');
      const chartReport = echarts.init(domChartReport, null, {
        renderer: 'canvas',
        useDirtyRect: false
      });

      const optionChartReport = {
        title: {
          text: ''
        },
        tooltip: {
          trigger: 'axis'
        },
        legend: {
          data: ['Air humidity', 'Soil humidity', 'Temperature']
        },
        grid: {
          left: '3%',
          right: '4%',
          bottom: '3%',
          containLabel: true
        },
        toolbox: {
          feature: {
            saveAsImage: {}
          }
        },
        xAxis: {
          type: 'category',
          boundaryGap: false,
          data: hourKeys,
        },
        yAxis: {
          type: 'value'
        },
        series: [{
          name: 'Air humidity',
          type: 'line',
          data: humidityAir
        }, {
          name: 'Soil humidity',
          type: 'line',
          data: humiditySoil
        }, {
          name: 'Temperature',
          type: 'line',
          data: temperature
        }]
      };

      if (optionChartReport && typeof optionChartReport === 'object') {
        chartReport.setOption(optionChartReport);
      }

      window.addEventListener('resize', chartReport.resize);
    }
  </script>

  <!-- Auto ON/OFF -->
  <script>
    document.getElementById('autoCheckbox').addEventListener('change', function() {
      const autoInfoDiv = document.querySelector('.auto-info');
      if (!this.checked) {
        autoInfoDiv.classList.add('disable');
      } else {
        autoInfoDiv.classList.remove('disable');
      }
    });
  </script>

  <!-- Device ON/OFF - Func -->
  <script type="text/javascript">
    function toggleAutoState(element) {
      const xhr = new XMLHttpRequest();
      if (element.checked) {
        xhr.open('GET', '/auto/on', true);
      } else {
        xhr.open('GET', '/auto/off', true);
      }
      xhr.send();
    }

    function toggleLight(element) {
      const xhr = new XMLHttpRequest();
      if (element.checked) {
        xhr.open('GET', '/light/on', true);
      } else {
        xhr.open('GET', '/light/off', true);
      }
      xhr.send();
    }

    function toggleFan(element) {
      const xhr = new XMLHttpRequest();
      if (element.checked) {
        xhr.open('GET', '/fan/on', true);
      } else {
        xhr.open('GET', '/fan/off', true);
      }
      xhr.send();
    }

    function toggleWater(element) {
      const xhr = new XMLHttpRequest();
      if (element.checked) {
        xhr.open('GET', '/water/on', true);
      } else {
        xhr.open('GET', '/water/off', true);
      }
      xhr.send();
    }
  </script>

  <!-- SETTING -->
  <script type="text/javascript">
    document.getElementById('tempForm').addEventListener('submit', function(event) {
      event.preventDefault();

      const minTemperature = document.getElementById('minTemperature').value;
      const maxTemperature = document.getElementById('maxTemperature').value;

      const params = new URLSearchParams();
      params.append('minTemperatureThreshold', minTemperature);
      params.append('maxTemperatureThreshold', maxTemperature);

      fetch('/setTemperature', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
          },
          body: params.toString()
        })
        .then(response => response.text())
        .then(data => {
          alert(data);
          initDataSetting();
        });
    });
  </script>
</body>

</html>
)html";

// LED
const int LED_PIN_D2_LIGHT = 4;
const int LED_PIN_D3_FAN = 0;
const int LED_PIN_D1_WATER = 5;

// State
bool autoState = true;
bool lightState = false;
bool fanState = false;
bool waterState = false;

// Value
float minTemperatureThreshold = 25.0;
float maxTemperatureThreshold = 30.0;
float minAirHumidityThreshold = 70.0;
float maxAirHumidityThreshold = 80.0;
float minSoilHumidityThreshold = 50.0;
float maxSoilHumidityThreshold = 70.0;

// URL
const char* serverUrl = "http://192.168.100.245:8088/api/device/status";

//
long getCurrentTimestamp() {
  time_t now = time(nullptr);
  return now;
}

void sendDeviceStatus(const String& deviceId, const String& deviceName, bool state) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(wifiClient, serverUrl);

    // Tạo JSON payload
    String jsonPayload = "{";
    jsonPayload += "\"deviceId\":\"" + deviceId + "\",";
    jsonPayload += "\"deviceName\":\"" + deviceName + "\",";
    jsonPayload += "\"state\":" + String(state ? "true" : "false") + ",";
    jsonPayload += "\"type\":" + String(autoState ? "true" : "false") + ",";
    jsonPayload += "\"datetime\":" + String(getCurrentTimestamp());
    jsonPayload += "}";

    http.addHeader("Content-Type", "application/json");

    Serial.print("Sending POST request to ");
    Serial.println(serverUrl);
    Serial.print("Payload: ");
    Serial.println(jsonPayload);

    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

void handleRoot() {
  server.send(200, "text/html", html);
}

// STATE --------------------------------------------------------------------------------------------------------------------------------------------------------
void handleAutoOn() {
  autoState = true;
  server.send(200, "text/plain", "AUTO is ON");
}

void handleAutoOff() {
  autoState = false;
  server.send(200, "text/plain", "AUTO is OFF");
}

// LIGHT --------------------------------------------------------------------------------------------------------------------------------------------------------
void handleLightOn() {
  lightState = true;
  digitalWrite(LED_PIN_D2_LIGHT, HIGH);
  sendDeviceStatus("1", "LIGHT", true);
  server.send(200, "text/plain", "LIGHT is ON");
}

void handleLightOff() {
  lightState = false;
  digitalWrite(LED_PIN_D2_LIGHT, LOW);
  sendDeviceStatus("1", "LIGHT", false);
  server.send(200, "text/plain", "LIGHT is OFF");
}

void toggleLight(bool action) {
  if (!autoState) {
    if (action) {
      handleLightOn();
    } else {
      handleLightOff();
    }
  } else {
    server.send(400, "text/plain", "AUTO is ON");
  }
}

// FAN ----------------------------------------------------------------------------------------------------------------------------------------------------------
void handleFanOn() {
  fanState = true;
  digitalWrite(LED_PIN_D3_FAN, HIGH);
  sendDeviceStatus("1", "FAN", true);
  server.send(200, "text/plain", "FAN is ON");
}

void handleFanOff() {
  fanState = false;
  digitalWrite(LED_PIN_D3_FAN, LOW);
  sendDeviceStatus("1", "FAN", false);
  server.send(200, "text/plain", "FAN is OFF");
}

void toggleFan(bool action) {
  if (!autoState) {
    if (action) {
      handleFanOn();
    } else {
      handleFanOff();
    }
  } else {
    server.send(400, "text/plain", "AUTO is ON");
  }
}

// WATER --------------------------------------------------------------------------------------------------------------------------------------------------------
void handleWaterOn() {
  waterState = true;
  digitalWrite(LED_PIN_D1_WATER, HIGH);
  sendDeviceStatus("1", "WATER", true);
  server.send(200, "text/plain", "WATER is ON");
}

void handleWaterOff() {
  waterState = false;
  digitalWrite(LED_PIN_D1_WATER, LOW);
  sendDeviceStatus("1", "WATER", false);
  server.send(200, "text/plain", "WATER is OFF");
}

void toggleWater(bool action) {
  if (!autoState) {
    if (action) {
      handleWaterOn();
    } else {
      handleWaterOff();
    }
  } else {
    server.send(400, "text/plain", "AUTO is ON");
  }
}

// ERROR --------------------------------------------------------------------------------------------------------------------------------------------------------
void handleNotFound() {
  server.send(404, "text/plain", "Page not found!");
}

// DEVICE ACTION ------------------------------------------------------------------------------------------------------------------------------------------------
void handleDeviceFan(float temperature, float airHumidity) {
  Serial.println("HandleDeviceFan   | Temperature : " + String(temperature) + " | TemperatureThreshold : " + String(minTemperatureThreshold) + " - " + String(maxTemperatureThreshold) + " | AutoState: " + String(autoState));
  Serial.println("HandleDeviceFan   | AirHumidity : " + String(airHumidity) + " | AirHumidityThreshold : " + String(minAirHumidityThreshold) + " - " + String(maxAirHumidityThreshold) + " | AutoState: " + String(autoState));
  
  if (((temperature > maxTemperatureThreshold) || (airHumidity > maxAirHumidityThreshold)) && autoState && !fanState) {
    Serial.println("FAN ON");
    handleFanOn();
  }
  if (((temperature < maxTemperatureThreshold) && (airHumidity < maxAirHumidityThreshold)) && autoState && fanState) {
    Serial.println("FAN OFF");
    handleFanOff();
  }
}

void handleDeviceWater(float soilHumidity) {
  Serial.println("HandleDeviceWater | SoilHumidity: " + String(soilHumidity) + " | SoilHumidityThreshold: " + String(minSoilHumidityThreshold) + " - " + String(maxSoilHumidityThreshold) + " | AutoState: " + String(autoState));
  
  if (soilHumidity > maxSoilHumidityThreshold && autoState && !waterState) {
    Serial.println("WATER ON");
    handleWaterOn();
  } 
  if (soilHumidity < minSoilHumidityThreshold && autoState && waterState) {
    Serial.println("WATER OFF");
    handleWaterOff();
  }
}

// SETTING ------------------------------------------------------------------------------------------------------------------------------------------------------
void handleSetTemperature() {
  if (server.hasArg("minTemperatureThreshold") && server.hasArg("maxTemperatureThreshold")) {
    minTemperatureThreshold = server.arg("minTemperatureThreshold").toFloat();
    maxTemperatureThreshold = server.arg("maxTemperatureThreshold").toFloat();

    String response = "Settings updated successfully!\n";

    server.send(200, "text/plain", response);
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

void getInitSettingValue() {
  server.send(200, "text/plain", String(minTemperatureThreshold) + '-' + String(maxTemperatureThreshold) + '-' + String(minAirHumidityThreshold) + '-' + String(maxAirHumidityThreshold) + '-' + String(minSoilHumidityThreshold) + '-' + String(maxSoilHumidityThreshold));
}

// GET SENSOR DATA ----------------------------------------------------------------------------------------------------------------------------------------------
void handleMonitor() {
  int soilMoistureValue = analogRead(SOIL_SENSOR_PIN);
  float soilMoisturePercent = map(soilMoistureValue, 0, 1023, 0, 100);

  float humidityAir = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidityAir) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  server.send(200, "text/plain", String(humidityAir) + '-' + String(soilMoisturePercent) + '-' + String(temperature) + '-' + String(lightState) + '-' + String(fanState) + '-' + String(waterState));
  handleDeviceFan(temperature, humidityAir);
  handleDeviceWater(soilMoisturePercent);
}

// SETUP SERVER -------------------------------------------------------------------------------------------------------------------------------------------------
void setupServer() {
  // Connect Wifi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Đồng bộ thời gian với server NTP
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  while (!time(nullptr)) {
    delay(1000);
    Serial.println("Waiting for time sync...");
  }
  Serial.println("Time synchronized");

  server.on("/", handleRoot);

   // STATE ------------------------------------------------------
  server.on("/auto/on", handleAutoOn);
  server.on("/auto/off", handleAutoOff);

  // LIGHT -------------------------------------------------------
  server.on("/light/on", []() {
    toggleLight(true);
  });
  server.on("/light/off", []() {
    toggleLight(false);
  });

  // FAN ---------------------------------------------------------
  server.on("/fan/on", []() {
    toggleFan(true);
  });
  server.on("/fan/off", []() {
    toggleFan(false);
  });

  // WATER --------------------------------------------------------
  server.on("/water/on", []() {
    toggleWater(true);
  });
  server.on("/water/off", []() {
    toggleWater(false);
  });

  // SENSOR -------------------------------------------------------
  server.on("/monitor", handleMonitor);
  server.on("/init/setting/value", getInitSettingValue);

  // SETTING ------------------------------------------------------
  server.on("/setTemperature", HTTP_POST, handleSetTemperature);

  // ERROR --------------------------------------------------------
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Webserver started");
}

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(LED_PIN_D2_LIGHT, OUTPUT);
  pinMode(LED_PIN_D3_FAN, OUTPUT);
  pinMode(LED_PIN_D1_WATER, OUTPUT);
  pinMode(SOIL_SENSOR_PIN, INPUT);
    
  dht.begin();

  // Off LED
  digitalWrite(LED_PIN_D2_LIGHT, LOW);
  digitalWrite(LED_PIN_D3_FAN, LOW);
  digitalWrite(LED_PIN_D1_WATER, LOW);

  setupServer();
}

void loop() {
  server.handleClient();
}

