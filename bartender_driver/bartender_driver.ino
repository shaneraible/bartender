// include the library code:
#include <LiquidCrystal.h>
#include "IRremote.h"
#include "FastLED.h"
#define NUM_LEDS 40
#define LED_PIN 5

CRGB leds[NUM_LEDS];
int NUM_BOTTLES=9;
int bottleLEDs[] = {4, 8, 12, 16, 21, 25, 29, 34, 39};
int IRPin = 11; //ir pin
int vodka = 22, rum = 23  , whiskey = 24, vermouth = 25; //alcohol pins
int lime = 26, cranberry = 27, ginger = 28, pineapple = 29, coke = 30; //micer pins

//times to get each to pump to top
int vodka_base=1000, rum_base=1000, whiskey_base = 1000, vermouth_base = 1000;
int lime_base = 1000, cranberry_base = 1000, ginger_base = 1000, pineapple_base = 1000, coke_base = 1000;

//pump rates 
int vodka_rate=1000, rum_rate=1000, whiskey_rate = 1000, vermouth_rate = 1000;
int lime_rate = 1000, cranberry_rate = 1000, ginger_rate = 1000, pineapple_rate = 1000, coke_rate = 1000;

IRrecv remote(IRPin);
decode_results selection;
LiquidCrystal lcd(2,3,4,5,6,7);
String drinks[] = {"0. Moscow Mule     ", "1. Vodka Cran      ", "2. Bay Breeze      ", "3. Rum & coke      ", "4. Old Fashioned     ", "5. Mai Tai         ", "6. Pineapple Rum      ", "7. Vodka             ", "8. Rum             ", "9. Tequila         "};
///maybe add mai tai later to replace the pure alcohol
int currentDrink = 0;

void setup(){
  Serial.begin(9600);
  remote.enableIRIn(); // Start the receiver

  pinMode(vodka, OUTPUT);
  pinMode(lime, OUTPUT);
  pinMode(cranberry, OUTPUT);
  pinMode(ginger, OUTPUT);
  currentDrink = 0;
  lcd.begin(16,2);
  lcd.setCursor(2,0);
  lcd.print("Select Drink");
  lcd.setCursor(0,1);
  lcd.print(drinks[currentDrink]);


  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness (20);
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  FastLED.show();
  delay(30);
  FastLED.setBrightness (255);
  FastLED.show();
}/*--(end setup )---*/

void loop(){
  if (remote.decode(&selection)){
    int pressed = translateIR(); 
    
    if(pressed!=-1){
      if(pressed%10==pressed)
        dispenseDrink(pressed);
      else{
        if(pressed==11) //code to dispense displayed drink
          dispenseDrink(currentDrink);
        else{
          lcd.clear();
          lcd.setCursor(2,0);
          lcd.print("Select Drink");          
          lcd.setCursor(0,1);
          
          if(pressed==10){
            currentDrink = (--currentDrink+10)%10;
            lcd.print(drinks[currentDrink]);
          }
          else if(pressed==12)
            currentDrink = (++currentDrink)%10;
            lcd.print(drinks[currentDrink]);
        }
        
      }
    }

    remote.resume();
  }  
}
void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}
void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}
void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}
int translateIR() //returns the selection pin # 
{
  switch(selection.value)
  {
    //case 0xFFA25D: Serial.println("POWER"); break;
    //case 0xFFE21D: Serial.println("FUNC/STOP"); break;
    //case 0xFF629D: Serial.println("VOL+"); break;
    case 0xFF22DD: return 10; // Serial.println("FAST BACK");    break;
    case 0xFF02FD: return 11; //Serial.println("PAUSE");    break;
    case 0xFFC23D: return 12; //Serial.println("FAST FORWARD");   break;
    //case 0xFFE01F: Serial.println("DOWN");    break;
    //case 0xFFA857: Serial.println("VOL-");    break;
    //case 0xFF906F: Serial.println("UP");    break;
    //case 0xFF9867: Serial.println("EQ");    break;
    //case 0xFFB04F: Serial.println("ST/REPT");  
    
    case 0xFF6897: return 0;
    case 0xFF30CF: return 1;
    case 0xFF18E7: return 2;
    case 0xFF7A85: return 3;
    case 0xFF10EF: return 4;
    case 0xFF38C7: return 5;
    case 0xFF5AA5: return 6;
    case 0xFF42BD: return 7;
    case 0xFF4AB5: return 8;
    case 0xFF52AD: return 9;
    //case 0xFFFFFFFF: Serial.println(" REPEAT");break;  

  }

  return -1;

}

void dispense(int drink, double size, double rate, double base){
  //time for size
  double t = rate*size + base;
  Serial.print(drink);
  digitalWrite(drink, HIGH);
  delay(t);
  digitalWrite(drink, LOW);
  delay(50);

}


void fadeInRange(int start, int fin, CRGB curr, CRGB color){
  for(int i=0; i<=100; i++){
    byte r = curr.r + i * (color.r - curr.r) / 100.0;
    byte g = curr.g + i * (color.g - curr.g) / 100.0;
    byte b = curr.b + i * (color.b - curr.b) / 100.0;
    CRGB fill(r,g,b);
    
    for(int k=start; k<fin; k++){
      leds[k] = fill;
    }
    FastLED.show();
  }
}

void dispenseDrink(int drink){
  lcd.clear();
  if(drink ==0){  //moscow mule
    lcd.setCursor(2,0);
    lcd.print("Moscow Mule");

    fadeInRange(0, 4, CRGB::Green, CRGB::Cyan);
    lcd.setCursor(0,1);
    lcd.print("Pouring vodka    ");
    dispense(vodka, 2, vodka_rate, vodka_base);
    fadeInRange(0, 4, CRGB::Cyan, CRGB::Green);

    fadeInRange(4, 8, CRGB::Green, CRGB::Cyan);
    lcd.setCursor(0,1);
    lcd.print("Pouring lime    ");
    dispense(lime, .75, lime_rate, lime_base);
    fadeInRange(4, 8, CRGB::Cyan, CRGB::Green);

    fadeInRange(19, 24, CRGB::Green, CRGB::Cyan);
    lcd.setCursor(0,1);
    lcd.print("disp. gingerbeer");
    dispense(ginger, 4, ginger_rate, ginger_base);
    fadeInRange(19, 24, CRGB::Cyan, CRGB::Green);
    
  }else if(drink == 1){ //vodka cranberry
    lcd.setCursor(3,0);
    lcd.print("Vodka Cran");
    
    lcd.setCursor(0,1);
    lcd.print("Pouring vodka    ");
    dispense(vodka, 1.5, vodka_rate, vodka_base);

    lcd.setCursor(0,1);
    lcd.print("Pouring lime    ");
    dispense(lime, .25, lime_rate, lime_base);

    lcd.setCursor(0,1);
    lcd.print("Pouring cranberry");
    dispense(cranberry, 4, cranberry_rate, cranberry_base);
    
  }else if(drink == 2){
    lcd.setCursor(2,0);
    lcd.print("Bay Breeze");

    lcd.setCursor(0,1);
    lcd.print("Pouring pineapple");
    dispense(pineapple, 2, pineapple_rate, pineapple_base);

    lcd.setCursor(0,1);
    lcd.print("Pouring vodka    ");
    dispense(vodka, 2, vodka_rate, vodka_base);

    lcd.setCursor(0,1);
    lcd.print("Pouring cranberry");
    dispense(cranberry, 3, cranberry_rate, cranberry_base);
    
    lcd.setCursor(0,1);
    lcd.print("Splash lime    ");
    dispense(lime, .25, lime_rate, lime_base);
  }else if(drink == 3){
    lcd.setCursor(2,0);
    lcd.print("RUM AND COKE");

    lcd.setCursor(0,1);
    lcd.print("Pouring rum");
    dispense(rum, 2, rum_base, rum_rate);

    lcd.setCursor(0,1);
    lcd.print("Pouring coke    ");
    dispense(coke, 4, coke_rate, coke_base);

    lcd.setCursor(0,1);
    lcd.print("Splash lime      ");
    dispense(lime, .25, lime_rate, lime_base);
  }else if(drink == 4){
    lcd.setCursor(2,0);
    lcd.print("NOT YET!");
  }else if(drink == 5){
    lcd.setCursor(2,0);
    lcd.print("NOT YET!");
  }else if(drink == 6){
    lcd.setCursor(2,0);
    lcd.print("PINEAPPLE RUM");

    lcd.setCursor(0,1);
    lcd.print("Pouring rum");
    dispense(rum, 2, rum_rate, rum_base);

    lcd.setCursor(0,1);
    lcd.print("Pouring pineapple");
    dispense(pineapple, 4, pineapple_rate, pineapple_base);

    lcd.setCursor(0,1);
    lcd.print("Splash lime      ");
    dispense(lime, .25, lime_rate, lime_base);
  }else if(drink ==7){
    lcd.setCursor(2,0);
    lcd.print("1 shot vodka");

    lcd.setCursor(0,1);
    lcd.print("Pouring..");
    dispense(vodka, 1.5, vodka_rate, vodka_base);
  }else if(drink == 8){
    lcd.setCursor(2,0);
    lcd.print("1 shot rum");

    lcd.setCursor(0,1);
    lcd.print("Pouring..");
    dispense(rum, 1.5, rum_rate, rum_base);
  }else{
    lcd.print("NOT YET!");
  }
  lcd.clear();
  lcd.print("     Enjoy!");
  delay(3500);
  setup();
}


void lightDrink(){
  
}

void turnOnDrinkLight(int drink){
  
}

void turnOffDrinkLight(int drink){
  
}
