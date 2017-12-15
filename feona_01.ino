/*******************
 * FOR NEOPIXELS
 ******************/
#include <math.h>
// This #include statement was automatically added by the Particle IDE.
#include <FastLED.h>
FASTLED_USING_NAMESPACE

#define LED_PIN     D0
#define NUM_LEDS    19
float BRIGHTNESS = 10;
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette;
TBlendType    currentBlending;
extern const TProgmemPalette16 myDefaultColorPalette_p PROGMEM;
extern const TProgmemPalette16 myCloudyColorPalette_p PROGMEM;
extern const TProgmemPalette16 myRainColorPalette_p PROGMEM;
extern const TProgmemPalette16 mySnowColorPalette_p PROGMEM;
extern const TProgmemPalette16 myNightColorPalette_p PROGMEM;
extern const TProgmemPalette16 myDayColorPalette_p PROGMEM;
/*******************
 * FOR TILT SENSOR
 ******************/
int tiltPin = D1;
int tiltState =0;
int lastTiltState =0;
int tiltMode = 0;

int lastTiltMode = 0;

/*******************
 * FOR DATA
 ******************/
uint8_t valueRec = 0;
uint8_t weatherIcon = 0;
float myTemp = 0;
int lastHour = 0;

void setup() {
 Serial.begin(115200); 
 Particle.subscribe("hook-response/get_weather", gotWeatherData, MY_DEVICES);

 for(int i=0;i<10;i++) {
        Serial.println("waiting " + String(10-i) + " seconds before we publish");
        delay(1000);
    }
    
 FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds,NUM_LEDS).setCorrection(TypicalLEDStrip);
 currentPalette =myDefaultColorPalette_p;
 currentBlending = LINEARBLEND;
 FastLED.setBrightness(BRIGHTNESS);
 
 /* FOR TILT SENSOR*/
 pinMode(tiltPin,INPUT);
}

void loop() {
/*triger the event hourly*/
int myHour = Time.hour();
if(myHour != lastHour){
     Particle.publish("get_weather");
     Serial.println("checking the weather!");
}
lastHour = myHour;

 /* TURN ON/TURN OFF*/
  tiltState = digitalRead(tiltPin);
  if(tiltState!=lastTiltState){
   if(tiltState == HIGH){
     Serial.println("changing tiltMode");
   }else{
     tiltMode = tiltMode +1;
     ledMode();
     Serial.println("current tiltMode is ");
     Serial.println(tiltMode);
     }
     delay(50);
   }
   lastTiltState = tiltState;

/*here is for testing different modes*?
//tiltMode = 1;
//if(tiltMode != lastTiltMode){
//    ledMode();
//}
//lastTiltMode = tiltMode;


/* NEOPIXELS DEFUALT SETTING */
 BRIGHTNESS =map((exp(sin(millis()/3000.0*M_PI)) - 0.36787944)*108.0,0,250,20,100) ;
 FastLED.setBrightness(BRIGHTNESS);
 static uint8_t startIndex = 0;
 startIndex = startIndex + 1; /* motion speed */
 FillLEDsFromPaletteColors( startIndex);
 FastLED.show();
 FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
   // uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
        colorIndex += 3;
    }
}

void gotWeatherData(const char *name, const char *data) {
    String str = String(data);

    weatherIcon = getValue(str, ',',0).toInt();
    myTemp = str.substring(2).toFloat();
    
    Serial.println(weatherIcon);
    Serial.println(myTemp);
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void ledMode(){
  
  switch(tiltMode){
    // case 0:
    // Serial.println("The lamp is turning off.");
    // //turnOff()
    // break;

    case 1:
    Serial.println("The lamp is turning on and show the weather icon.");
    weatherICON();
    break;

    case 2:
    Serial.println("The lamp is turning on and show the temp.");
    showTemp(myTemp);
    break;

    default:
    Serial.println("The lamp is just turned off now.");
    clearUp();
    currentPalette = myDefaultColorPalette_p;
    break;    
  }
}

void showTemp(float data){
    
    int R = 50;
    int B = 50;
    int x = floor(data);
    
    //int x =floor(-13);
    
    
    if(x >40){
        R = map(x,-60,110,50,255);
        Serial.println(R);
        for( int i = 0; i < 16; i++) {
            currentPalette[i] = CRGB( R, 100, B);
        }
    }else if(x<=40){
        B = map(x,-60,110,50,255);
        Serial.println(B);
        for( int i = 0; i < 16; i++) {
            currentPalette[i] = CRGB( R, 100, B);
        }
    }
}

void weatherICON(){
  switch(weatherIcon){
    case 1:
    Serial.println("clearly day");
    currentPalette = myDayColorPalette_p;
    break;

    case 2:
    Serial.println("cloudy/partly cloudy day/partly cloudy night");
    //SetupCloudyColor();
    currentPalette = myCloudyColorPalette_p;
    break;

    case 3:
    Serial.println("clear night");
    currentPalette = myNightColorPalette_p;
    break;
    
    case 4:
    Serial.println("rain");
    currentPalette = myRainColorPalette_p;
    break;
    
    case 5:
    Serial.println("snow.");
    currentPalette = mySnowColorPalette_p;
    break;

    default:
    Serial.println("null");
    currentPalette = myDefaultColorPalette_p;
    break;    
  }
}

void clearUp(){
  tiltMode = 0;
}

const TProgmemPalette16 myDayColorPalette_p PROGMEM =
{
    CRGB::LightSkyBlue,
    CRGB::OldLace, 
    CRGB::Yellow,
    CRGB::Orange,
    
    CRGB::LightSkyBlue,
    CRGB::OldLace, 
    CRGB::Yellow,
    CRGB::Orange,
    
    CRGB::LightSkyBlue,
    CRGB::OldLace, 
    CRGB::Yellow,
    CRGB::Orange,
    
    CRGB::LightSkyBlue,
    CRGB::OldLace, 
    CRGB::Yellow,
    CRGB::Orange
};

const TProgmemPalette16 myCloudyColorPalette_p PROGMEM =
{
    CRGB::DimGray,
    CRGB::DimGray, 
    CRGB::DimGray,
    CRGB::DimGray,
    
    CRGB::DimGray,
    CRGB::DimGray, 
    CRGB::DimGray,
    CRGB::DimGray,
    
    CRGB::DimGray,
    CRGB::DimGray, 
    CRGB::DimGray,
    CRGB::DimGray,
    
    CRGB::DimGray,
    CRGB::DimGray, 
    CRGB::DimGray,
    CRGB::DimGray
};

const TProgmemPalette16 myNightColorPalette_p PROGMEM =
{
    CRGB::MidnightBlue,
    CRGB::MidnightBlue, 
    CRGB::MidnightBlue,
    CRGB::MidnightBlue,
    
    CRGB::MidnightBlue,
    CRGB::MidnightBlue, 
    CRGB::MidnightBlue,
    CRGB::MidnightBlue,
    
    CRGB::MidnightBlue,
    CRGB::MidnightBlue, 
    CRGB::MidnightBlue,
    CRGB::MidnightBlue,
    
    CRGB::MidnightBlue,
    CRGB::MidnightBlue, 
    CRGB::MidnightBlue,
    CRGB::MidnightBlue
};

const TProgmemPalette16 myRainColorPalette_p PROGMEM =
{
    CRGB::Turquoise,
    CRGB::White, 
    CRGB::White,
    CRGB::SkyBlue,
    
    CRGB::Turquoise,
    CRGB::White, 
    CRGB::White,
    CRGB::SkyBlue,
    
    CRGB::Turquoise,
    CRGB::White, 
    CRGB::White,
    CRGB::SkyBlue,
    
    CRGB::Turquoise,
    CRGB::White, 
    CRGB::White,
    CRGB::SkyBlue
};
    
const TProgmemPalette16 mySnowColorPalette_p PROGMEM =
{
    CRGB::Beige,
    CRGB::Beige, 
    CRGB::SlateGray,
    CRGB::Beige,
    
    CRGB::Beige,
    CRGB::Beige, 
    CRGB::SlateGray,
    CRGB::Beige,
    
    CRGB::Beige,
    CRGB::Beige, 
    CRGB::SlateGray,
    CRGB::Beige,
    
    CRGB::Beige,
    CRGB::Beige, 
    CRGB::SlateGray,
    CRGB::Beige
};

const TProgmemPalette16 myDefaultColorPalette_p PROGMEM =
{
    CRGB::Gray,
    CRGB::Gray, 
    CRGB::Gray,
    CRGB::Gray,
    
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Gray,
    
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Gray,
    
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Gray
};

// void ChangePalette()
// {
//     uint8_t valueHold = valueRec;
//     uint8_t lastValue;

//     if(lastValue != valueHold){
//       lastValue = valueHold;
//       if(valueHold == 0){Serial.println(valueHold);}
//       if(valueHold == 2){Serial.println(valueHold);}
//       //....
//     }else{
//       Serial.println("No updates!");
//       Serial.println(valueHold);
//     }
// }
