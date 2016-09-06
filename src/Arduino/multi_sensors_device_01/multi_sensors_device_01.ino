//
//  초음파와 온도/습도센서
//
#include <DHT11.h>    //라이브러리 불러옴

//초음파 센서의 핀번호를 설정한다.
int ultrasonic_echo_pin = 12;
int ultrasonic_triger_Pin = 13;

//  온도/습도
int temperature_humidity_sensor_pin = 2;  //Signal 이 연결된 아두이노의 핀번호

DHT11 dht11(temperature_humidity_sensor_pin);

void setup() {
  Serial.begin(9600);
  
  // trig를 출력모드로 설정, echo를 입력모드로 설정
  pinMode(ultrasonic_triger_Pin, OUTPUT);
  pinMode(ultrasonic_echo_pin, INPUT);
}

void loop() {
  int err;
  float us_duration, us_distance;
  float temperature, humidity;
  
  char  json_data_buf[256];
  char  convert_float_buf[2][10];

  // 초음파
  {	
     // 초음파를 보낸다. 다 보내면 echo가 HIGH 상태로 대기하게 된다.
    digitalWrite(ultrasonic_triger_Pin, HIGH);
    delay(10);
    digitalWrite(ultrasonic_triger_Pin, LOW);
  	
    // echoPin 이 HIGH를 유지한 시간을 저장 한다.
    us_duration = pulseIn(ultrasonic_echo_pin, HIGH); 
    // HIGH 였을 때 시간(초음파가 보냈다가 다시 들어온 시간)을 가지고 거리를 계산 한다.
    us_distance = ((float)(340 * us_duration) / 10000) / 2;
  
    dtostrf(us_distance, 6, 3, convert_float_buf[0]);
    
    sprintf(json_data_buf, 
      "{"\
      "   \"device\": {"\
      "      \"type\": \"ultrasonic_sensor\","\
      "      \"distance\": %s"\
      "   }"\
      "}", convert_float_buf[0]);     
      
    Serial.println(json_data_buf);
  }
  
  if ( (err = dht11.read(humidity, temperature) )==0) { //온도, 습도 읽어와서 표시
  
    dtostrf(temperature, 4, 1, convert_float_buf[0]);
    dtostrf(humidity, 4, 1, convert_float_buf[1]);
    
    sprintf(json_data_buf, 
      "{"\
      "   \"device\": {"\
      "      \"type\": \"temperature_sensor\","\
      "      \"temperature\": %s,"\
      "      \"humidity\": %s"\
      "   }"\
      "}", 
      convert_float_buf[0], 
      convert_float_buf[1]); 
      
    Serial.println(json_data_buf);
  }

  delay(500);
}
