#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

#define DHTPIN D3
#define DHTTYPE DHT11
#define TRIGPIN D1
#define ECHOPIN D2
#define REDLED D4
#define GREENLED D5
#define BUZZER D6

DHT dht(DHTPIN, DHTTYPE);

#define FIREBASE_HOST "https://kurs-5b897-default-rtdb.europe-west1.firebasedatabase.app/"  
#define FIREBASE_AUTH "AIzaSyDmG8EaBb9mdeyvAhkuP3hUL4LFtaGNGa4"

#define WIFI_SSID "IPhone 14 pro"
#define WIFI_PASSWORD "ensarensar"

const char* ssid = "IPhone 14 pro";
const char* password = "ensarensar";


FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData;


void setup() {


  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Povezivanje na WiFi...");
  }
  Serial.println("Povezano!");

  
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  
    Firebase.begin(&config, &auth);
   
  if (Firebase.ready()) {
    Serial.println("Povezano na Firebase!");
  } else {
    Serial.println("Firebase konekcija nije uspjela.");
    Serial.println(firebaseData.errorReason());
  }

  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  pinMode(REDLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  dht.begin();
}

float getDistance() {
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);

  long duration = pulseIn(ECHOPIN, HIGH);
  return (duration * 0.034) / 2;
}

void loop() {
  float temp = dht.readTemperature();
  float dist = getDistance();

  if (temp > 25) {
    digitalWrite(REDLED, HIGH);
    digitalWrite(GREENLED, LOW);
    if (dist < 15) {
      tone(BUZZER, 659);
    } else {
      noTone(BUZZER);
    }
  } else {
    digitalWrite(REDLED, LOW);
    digitalWrite(GREENLED, HIGH);
    digitalWrite(BUZZER, LOW);
  }

 
  if (Firebase.setFloat(firebaseData, "/temperature", temp)) {
  Serial.println("Temperature sent to Firebase!");
} else {
  Serial.print("Failed to send temperature: ");
  Serial.println(firebaseData.errorReason());
}

if (Firebase.setFloat(firebaseData, "/distance", dist)) {
  Serial.println("Distance sent to Firebase!");
} else {
  Serial.print("Failed to send distance: ");
  Serial.println(firebaseData.errorReason());
}


if (Firebase.getBool(firebaseData, "/buzzer")) {
    if (firebaseData.boolData()) { 
      tone(BUZZER,659);
    } else {
      noTone(BUZZER); 
    }
  } else {
    Serial.println("Greška pri čitanju stanja buzzera!");
    noTone(BUZZER); 
  }


  delay(2000);
}
