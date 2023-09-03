#include <WiFi.h>
#include <ArduinoJson.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#define rainAnalog 35

Adafruit_MPU6050 mpu;
float gravity = 9.89;
float ax, ay, az;
float gx, gy, gz;
int humidity=0;


const char* ssid = "CLARO_2GC9ADAD";
const char* password = "38C9ADAD";
const char* serverAddress = "8a4a-34-82-32-23.ngrok-free.app";
const int serverPort = 80;
const String endpoint = "/data_with_humidity"; // Change this to the appropriate endpoint on your server

float samples[7]; // 6 values per sample (x, y, z for acceleration and inclination)
float accelerationX =0;
float accelerationY = 0;
float accelerationZ =0;  
float inclinationX =0;
float inclinationY =0;
float inclinationZ =0;

void setup() {
   
  Serial.begin(9600);
  delay(100);
  setup_mpu();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
 
  for (int i = 0; i < 7; i++) {
      samples[i] = 0.0;

  }
  delay(3000);

}


void loop() {
  // Simulate collecting data (replace with your actual data collection code)
  get_mpu();
  get_humidity();
 
  samples[0] = ax;
  samples[1] = ay;
  samples[2] = az;
  samples[3] = gx;
  samples[4] = gy;
  samples[5] = gz;
  samples[6] = humidity;
  String resultado = sendDataToServer();
  Serial.println(resultado);
}
String sendDataToServer() {
  WiFiClient client;
  String result;
  if (client.connect(serverAddress, serverPort)) {
    DynamicJsonDocument jsonDoc(4096);

    JsonObject sampleObj = jsonDoc.createNestedObject();
    sampleObj["AccX"] = samples[0];
    sampleObj["AccY"] = samples[1];
    sampleObj["AccZ"] = samples[2];
    sampleObj["GyroX"] = samples[3];
    sampleObj["GyroY"] = samples[4];
    sampleObj["GyroZ"] = samples[5];
    sampleObj["Humidity"] = samples[6];
    String payload;
    serializeJson(jsonDoc, payload);

    client.println("POST " + endpoint + " HTTP/1.1");
    client.println("Host: " + String(serverAddress));
    client.println("Content-Type: application/json");
    client.println("Content-Length: " + String(payload.length()));
    client.println();
    client.println(payload);
    unsigned long timeout = millis(); // Get the current time

    // Read and print the response from the server



      while (client.connected() && (millis() - timeout) < 2000) {
        if (client.available()) {
          char c = client.read();
           //Serial.write(c);

          // Collect the characters until a closing curly brace is encountered
          if (c == '}') {
            result += c; // Include the closing curly brace in the result
            break; // Exit the loop
          }
          result += c; // Append characters to the result
        }
      }
  String result_corrected =result.substring(217, result.length()-2);

  return result_corrected;
  }

}

void setup_mpu(){
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
  get_mpu();
  delay(2000);
}

void get_humidity(){
  humidity = random(0,4095);
  
 // int rainAnalogVal = analogRead(rainAnalog);

}
void get_mpu(){
   sensors_event_t a, g, temp;
   mpu.getEvent(&a, &g, &temp);
   ax=a.acceleration.x;
   ay=a.acceleration.y;
   az= a.acceleration.z -gravity;
   gx=g.gyro.x;
   gy=g.gyro.y;
   gz=g.gyro.z;
}