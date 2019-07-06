#include <Wire.h>
#include <DHT.h>
#include <DS1302.h>
#include <SD.h>
#include <SPI.h>
File myFile;
DS1302 rtc(8,7,6);
Time t;
int buzzer=3;
int pinCS = 10;
int contador = 0;
DHT dht(4, DHT11);
int gyro_x, gyro_y, gyro_z;
long acc_x, acc_y, acc_z, acc_total_vector;
int temperature;
long gyro_x_cal, gyro_y_cal, gyro_z_cal;
long loop_timer;
int lcd_loop_counter;
float angle_pitch, angle_roll;
int angle_pitch_buffer, angle_roll_buffer;
boolean set_gyro_angles;
float angle_roll_acc, angle_pitch_acc;
float angle_pitch_output, angle_roll_output;
bool maltrato=false;
float old_gyro_x, old_gyro_y, old_gyro_z, old_acc_x, old_acc_y, old_acc_z;
float difgyrox,difgyroy,difgyroz,difaccx,difaccy,difaccz;
long buzzer_interval=1000;
unsigned long buzzerMillis=0;
int buzzer_state=262;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(pinCS,OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(5, INPUT);
  if(SD.begin()){
    Serial.println("contador,fecha,hora,humedad,temperatura,acelX,acelY,acelZ,gyroX,gyroY,gyroZ,pitch,roll,maltrato");
  }
  else{
    Serial.println("La tarjeta SD no pudo iniciar correctamente");
  }
  dht.begin();
  setup_mpu_9250_registers();
  digitalWrite(2, HIGH);
  Serial.println("Calibrating gyro");
  for(int cal_int=0; cal_int<2000; cal_int ++){
    if(cal_int%125==0)Serial.print(".");
    read_mpu_9250_data();                                              //Read the raw acc and gyro data from the MPU-6050
    gyro_x_cal += gyro_x;                                              //Add the gyro x-axis offset to the gyro_x_cal variable
    gyro_y_cal += gyro_y;                                              //Add the gyro y-axis offset to the gyro_y_cal variable
    gyro_z_cal += gyro_z;                                              //Add the gyro z-axis offset to the gyro_z_cal variable
    delay(3);
  }
  gyro_x_cal /= 2000;                                                  //Divide the gyro_x_cal variable by 2000 to get the avarage offset
  gyro_y_cal /= 2000;                                                  //Divide the gyro_y_cal variable by 2000 to get the avarage offset
  gyro_z_cal /= 2000;
  Serial.println("");
  Serial.println("Pitch,Roll,Ax,Ay,Az,Gx,Gy,Gz");
  digitalWrite(2,LOW);
  loop_timer=micros();
}

void loop() {
  contador=contador+1;
  t=rtc.getTime();
  myFile=SD.open("bebe.txt", FILE_WRITE);
  if(contador==1) myFile.println("contador,fecha,hora,humedad,temperatura,acelX,acelY,acelZ,gyroX,gyroY,gyroZ,pitch,roll,maltrato");
  read_mpu_9250_data();
  gyro_x -= gyro_x_cal;                                                //Subtract the offset calibration value from the raw gyro_x value
  gyro_y -= gyro_y_cal;                                                //Subtract the offset calibration value from the raw gyro_y value
  gyro_z -= gyro_z_cal;
  angle_pitch += gyro_x * 0.0000611;                                   //Calculate the traveled pitch angle and add this to the angle_pitch variable
  angle_roll += gyro_y * 0.0000611;
  angle_pitch += angle_roll * sin(gyro_z * 0.000001066);               //If the IMU has yawed transfer the roll angle to the pitch angel
  angle_roll -= angle_pitch * sin(gyro_z * 0.000001066);
  //Accelerometer angle calculations
  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));  //Calculate the total accelerometer vector
  //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
  angle_pitch_acc = asin((float)acc_y/acc_total_vector)* 57.296;       //Calculate the pitch angle
  angle_roll_acc = asin((float)acc_x/acc_total_vector)* -57.296;       //Calculate the roll angle
  angle_pitch_acc -= 0.0;                                              //Accelerometer calibration value for pitch
  angle_roll_acc -= 0.0;                                               //Accelerometer calibration value for roll
  if(set_gyro_angles){                                                 //If the IMU is already started
    angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;     //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
    angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;        //Correct the drift of the gyro roll angle with the accelerometer roll angle
  }
  else{                                                                //At first start
    angle_pitch = angle_pitch_acc;                                     //Set the gyro pitch angle equal to the accelerometer pitch angle 
    angle_roll = angle_roll_acc;                                       //Set the gyro roll angle equal to the accelerometer roll angle 
    set_gyro_angles = true;                                            //Set the IMU started flag
  }
  //To dampen the pitch and roll angles a complementary filter is used
  angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
  angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1;

  if(angle_pitch_output>30||angle_pitch_output<-30||angle_roll_output>30||angle_roll_output<-30){
    maltrato=true;
  }
  difgyrox=abs(old_gyro_x-gyro_x);
  difgyroy=abs(old_gyro_y-gyro_y);
  difgyroz=abs(old_gyro_z-gyro_z);
  difaccx=abs(old_acc_x-acc_x);
  difaccy=abs(old_acc_y-acc_y);
  difaccz=abs(old_acc_z-acc_z);
  old_gyro_x=gyro_x;                                                   //Anterior valor
  old_gyro_y=gyro_y;
  old_gyro_z=gyro_z;
  old_acc_x=acc_x;
  old_acc_y=acc_y;
  old_acc_z=acc_z;
  if(difgyrox>10000||difgyroy>10000||difgyroz>10000){
    maltrato=true;
  }
  if(difaccx>10000||difaccy>10000||difaccz>10000){
    maltrato==true;
  }
  if(t.hour==3&&t.min==0&&t.sec==0){
    maltrato=true;
  }
  if(t.hour==6&&t.min==0&&t.sec==0){
    maltrato=true;
  }
  if(t.hour==9&&t.min==0&&t.sec==0){
    maltrato=true;
  }
  if(t.hour==15&&t.min==0&&t.sec==0){
    maltrato=true;
  }
  if(t.hour==18&&t.min==0&&t.sec==0){
    maltrato=true;
  }
  if(t.hour==21&&t.min==0&&t.sec==0){
    maltrato=true;
  }
  if(dht.readTemperature()>35){
    maltrato=true;
  }
  if(dht.readTemperature()<18){
    maltrato=true;
  }
  if(digitalRead(5)){
    maltrato=false;
    noTone(buzzer);
    digitalWrite(2, LOW);
  }
  if(maltrato==true){
    if(millis()-buzzerMillis>=buzzer_interval){
      buzzerMillis=millis();
      digitalWrite(2, !digitalRead(2));
      tone(buzzer, buzzer_state);
      delay(1000);
    }
  }

  myFile.print(contador);
  myFile.print(",");
  myFile.print(t.date);
  myFile.print("/");
  myFile.print(t.mon,DEC);
  myFile.print("/");
  myFile.print(t.year);
  myFile.print(",");
  myFile.print(t.hour);
  myFile.print(":");
  myFile.print(t.min);
  myFile.print(":");
  myFile.print(t.sec);
  myFile.print(",");
  myFile.print(dht.readHumidity());
  myFile.print(",");
  myFile.print(dht.readTemperature());
  myFile.print(",");
  myFile.print(acc_x);
  myFile.print(",");
  myFile.print(acc_y);
  myFile.print(",");
  myFile.print(acc_z);
  myFile.print(",");
  myFile.print(gyro_x);
  myFile.print(",");
  myFile.print(gyro_y);
  myFile.print(",");
  myFile.print(gyro_z);
  myFile.print(",");
  myFile.print(angle_pitch_output);
  myFile.print(",");
  myFile.print(angle_roll_output);
  myFile.print(",");
  myFile.println(maltrato);
  myFile.close();
  
  Serial.print(contador);
  Serial.print("\t");
  Serial.print(t.date);
  Serial.print("/");
  Serial.print(t.mon,DEC);
  Serial.print("/");
  Serial.print(t.year);
  Serial.print("\t");
  Serial.print(t.hour);
  Serial.print(":");
  Serial.print(t.min);
  Serial.print(":");
  Serial.print(t.sec);
  Serial.print("\t");
  Serial.print(dht.readHumidity());
  Serial.print("\t");
  Serial.print(dht.readTemperature());
  Serial.print("\t");
  Serial.print(acc_x);
  Serial.print("\t");
  Serial.print(acc_y);
  Serial.print("\t");
  Serial.print(acc_z);
  Serial.print("\t");
  Serial.print(gyro_x);
  Serial.print("\t");
  Serial.print(gyro_y);
  Serial.print("\t");
  Serial.print(gyro_z);
  Serial.print("\t");
  Serial.print(angle_pitch_output);
  Serial.print("\t");
  Serial.print(angle_roll_output);
  Serial.print("\t");
  Serial.println(maltrato);

  old_gyro_x=gyro_x;
  old_gyro_y=gyro_y;
  old_gyro_z=gyro_z;
  old_acc_x=acc_x;
  old_acc_y=acc_y;
  old_acc_z=acc_z;
  
  while(micros()-loop_timer<4000);
  loop_timer=micros();
}

void read_mpu_9250_data(){                                             //Subroutine for reading the raw gyro and accelerometer data
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x3B);                                                    //Send the requested starting register
  Wire.endTransmission();                                              //End the transmission
  Wire.requestFrom(0x68,14);                                           //Request 14 bytes from the MPU-6050
  while(Wire.available() < 14);                                        //Wait until all the bytes are received
  acc_x = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_x variable
  acc_y = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_y variable
  acc_z = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_z variable
  temperature = Wire.read()<<8|Wire.read();                            //Add the low and high byte to the temperature variable
  gyro_x = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_x variable
  gyro_y = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_y variable
  gyro_z = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_z variable
}

void setup_mpu_9250_registers(){
  //Activate the MPU-6050
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x6B);                                                    //Send the requested starting register
  Wire.write(0x00);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the accelerometer (+/-8g)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1C);                                                    //Send the requested starting register
  Wire.write(0x10);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the gyro (500dps full scale)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1B);                                                    //Send the requested starting register
  Wire.write(0x08);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
}
