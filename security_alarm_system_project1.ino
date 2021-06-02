#include <LiquidCrystal.h> // Include library for LCD.
#include <Keypad.h> // Include keypad library.

int buzzing = 10000;   // buzz after period (ms)
//for the rgbLED
const byte red =4;
const byte gre =3;
//digitalWrite(?, HIGH/LOW);


// for the keymap.
const byte rows = 4; // Four rows.
const byte cols = 4; // Four columns.
char keys[rows][cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[rows] = {2,12,11,10}; 
byte colPins[cols] = {9,8,7,6}; 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);


// for the LCD
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
int LCDRow =0;
//lcd.setCursor(x,y);
//lcd.print(" :) "); used for printing


// for the PIR
int sensor = 13; 
int state = 0; // State of the PIR sensor.
bool Detected = false; // we need 1 or 0 so bool is enough.
bool locked = false; // 

// buzzer
int buzzer = 5;

String passcode = "753";
String input = ""; //from keypad

int push =0;
char key;

  void setup()
{
  	pinMode (red,    OUTPUT);
  	pinMode (gre,    OUTPUT);
  	pinMode (sensor, INPUT);
    pinMode (buzzer, OUTPUT);
  	pinMode (push,    INPUT);
    lcd.begin(16,2);
  }


void loop()
{
	if (locked == false)
    {
  	digitalWrite(gre, HIGH);
   	digitalWrite(red, LOW);
  	lcd.setCursor(0,0);
 	lcd.print("Sifre Gir");
	char key = keypad.getKey();
    	if (key && key!= '#')
        {
        input +=key;
    	Serial.println(key);
    	lcd.setCursor (LCDRow, 1);
    	lcd.print(key);
    	lcd.setCursor (++LCDRow, 1);
        }
      		if (key == '#')
            {
              if (passcode == input)
              {
              	digitalWrite(gre, LOW);
              	digitalWrite(red, HIGH);
              	locked = true;
                input = "";
              	lcd.clear();
      			lcd.print("Hatalı Giriş");
        		lcd.setCursor(0,1);
        		lcd.print("ARMED");
                delay(3000);
                lcd.clear();
              }
              else{
                input = "";
              	lcd.clear();
      			lcd.print("Tekrar Dene");
                lcd.setCursor(0,1);
             	LCDRow =0;
              	delay(1000);}
            }	
              
	}
	else if (locked == true)
    {
      state = digitalRead(sensor);
      int bstate = digitalRead(push);
      LCDRow =0;
      if (state || bstate)
      {
        digitalWrite(gre, HIGH);
        lcd.setCursor(0,0);
      	lcd.print("Alarm! xxx#?");
        Detected= true;
      }
      while (Detected==true)
      {
        char key = keypad.getKey();
		// 10 seconds then buzzer
 		setTimerInterrupt(10000);
    	if (key && key!= '#')
        {
        input +=key;
    	Serial.println(key);
    	lcd.setCursor (LCDRow, 1);
    	lcd.print(key);
    	lcd.setCursor (++LCDRow, 1);
        }
       	 if (key == '#')
            {
              if (passcode == input)
              {
               buzzing = 10000;
               noTone(buzzer);
               Detected=false;
               locked=false;
               input ="";
               lcd.clear();
              }
           		else
                  {
                  delay(100);
                  alarm();
                  Detected=true;
               	  locked=true;
                  input ="";
                  lcd.clear();
                  lcd.print("Yanlış! Tekrar Dene");
                  delay(1000);
                  }
      		}
    	
      }
    }
  
}
 void alarm(){
   tone(buzzer, 500);}
void setTimerInterrupt(long uSecs) {
  noInterrupts();         
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  // compare match register 16MHz/256 * t(s) - 1
  OCR1A = (16e6 / 256L * uSecs) / 1e6 - 1;            

  TCCR1B |= (1 << WGM12);   
  TCCR1B |= (1 << CS12);    
  TIMSK1 |= (1 << OCIE1A); 
  interrupts();             // enable all interrupts
}
ISR(TIMER1_COMPA_vect) {
   if (buzzing)
    buzzing--;
  if(buzzing==0)
  alarm();
}