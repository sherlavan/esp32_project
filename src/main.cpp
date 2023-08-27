#include <Arduino.h>
#include <TM1637Display.h>
#include <WiFi.h>


#define LED 2 // onboard LED
#define EARTH_HUM 36 //analog in hum earth sensor
int earthHumValue = 0; //1300(100%) - water; 2700(0%) - air
int minEHV = 1300;
int maxEHV = 3333;
int earthHumPP = 0;
int minEarthHum = 50; //Granitsa na4ala poliva %%
int maxEarthHum = 80; //Granitsa okon4ani9l poliva %%
#define PUMP 12 //pwm pin to connect power relay for pump

#define DISP_CLk 27 //tm1637 display
#define DISP_DIO 14


TM1637Display disp = TM1637Display(DISP_CLk, DISP_DIO);


const uint8_t SEG_DONE[] = {
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_C | SEG_E | SEG_G,                           // n
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
	};

const uint8_t SEG_err1[] = {
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,           // E
	SEG_E | SEG_G ,                                  // r
	SEG_E | SEG_G ,                                  // r
	SEG_B | SEG_C                                    // 1
	};

const uint8_t CELC[] = {
  SEG_A | SEG_B | SEG_F | SEG_G,  // Degree symbol
  SEG_A | SEG_D | SEG_E | SEG_F   // C
};

void startPump(){

  digitalWrite(PUMP, HIGH);

}

void stopPump(){

  digitalWrite(PUMP, LOW);

}

int earthHumInPP(int val){
  int res = 0;
  if (val > maxEHV) return val;
  if (val < minEHV) return val;
  res = 100 - (int)(100*(val - minEHV)/(maxEHV - minEHV));
  return res;

}


void setup() {
  WiFi.mode(WIFI_OFF);
  btStop();
  // analogWrite(EARTH_HUM, 0);
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print(adcAttachPin(EARTH_HUM));

  // pinMode(EARTH_HUM, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(PUMP, OUTPUT);
  digitalWrite(PUMP, LOW);
  // analogSetPinAttenuation(EARTH_HUM, ADC_11db);
  disp.setBrightness(7);
  disp.setSegments(SEG_DONE);

  
}

void loop() {
  earthHumValue = analogRead(EARTH_HUM);
  earthHumPP = earthHumInPP(earthHumValue);

  if (earthHumPP < minEarthHum){
    digitalWrite(LED, HIGH);
    startPump();
  }
  if (earthHumPP > maxEarthHum){
    digitalWrite(LED, LOW);
    stopPump();
  }

  if (earthHumPP > 111){
    Serial.print("Error in EarthHUM sensor, analog pin is ");
    Serial.print(EARTH_HUM);
    Serial.print(", Value of sens is: ");
    Serial.print(earthHumValue);
    Serial.println(". Should be in ~900 - 3000");
    disp.setSegments(SEG_err1);

    delay(2000);
  }
  
  
  Serial.print(earthHumPP);
  Serial.print("  ");
  Serial.println(earthHumValue);
  disp.showNumberDec(earthHumPP);
  delay(1000);
  // disp.setSegments(CELC);

  // put your main code here, to run repeatedly:
}