//-----setting-----//
#define debugMode 1
#define invert 1
//-----setting-----//


//-----lib & define & init-----//
#if debugMode == 1
#define debug(x) Serial.println(x)
#else 
#define debug(x) 
#endif

#if invert == 1
#define setPin(pin, x) digitalWrite(pin, !x)
#else
#define setPin(pin, x) digitalWrite(pin, x)
#endif

#define __A 0
#define __B 1
#define __C 2
#define __D 3
#define __E 4
#define __F 5
#define __G 6
//-----lib & define & init-----//


//-----special variables-----//
volatile uint8_t segments[] = { 6,7,8,9,10,11,12 }; //пины сегментов (A B C D E F G)
volatile uint8_t anod[] = { 2,3,4,5 };  //пины разрядов
volatile uint8_t buf[4];    //буфер дисплея
//-----special variables-----//


//-----func-----//
#pragma region
void setAnod(uint8_t num) {
    num = 0x01 << num;
    digitalWrite(anod[0], bitRead(num, 0));
    digitalWrite(anod[1], bitRead(num, 1));
    digitalWrite(anod[2], bitRead(num, 2));
    digitalWrite(anod[3], bitRead(num, 3));
}

void setDigit(uint8_t num) {
    /*
    //0b00111111  => 0
    //0b00000110  => 1
    //0b01011011  => 2
    //0b01001111  => 3
    //0b01100110  => 4

    //0b01101101  => 5
    //0b01111101  => 6
    //0b00000111  => 7
    //0b01111111  => 8
    //0b01101111  => 9
    //uint8_t arr[] = { 0x7F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0xFF, 0x6F };
    */
    uint8_t arr[] = { 0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111 };
    for (uint8_t i = 0;i < 7;i++)setPin(segments[i], bitRead(arr[num], i));
}

void setSegment(uint8_t x) {
    x = 0x01 << x;
    /*
    setPin(segments[__A], bitRead(x, 0));
    setPin(segments[__B], bitRead(x, 1));
    setPin(segments[__C], bitRead(x, 2));
    setPin(segments[__D], bitRead(x, 3));
    setPin(segments[__E], bitRead(x, 4));
    setPin(segments[__F], bitRead(x, 5));
    setPin(segments[__G], bitRead(x, 6));
    */
    for (uint8_t i = 0;i < 7;i++)setPin(segments[i], bitRead(x, i));
}

void sptlit(uint16_t num) {
    for (int8_t i = 3;i >= 0;i--) {
        buf[i] = num % 10;
        num /= 10;
    }
}


ISR(TIMER2_COMPA_vect) {
    static uint8_t counter = 0;
    static uint8_t i = 0;
    if (++counter == 20)digitalWrite(anod[i], false);

    if (counter > 21) {
        counter = 0;
        if (++i >= 4)i = 0;

        setDigit(buf[i]);
        digitalWrite(anod[i], true);
    }
}

/*
void display() {
    static uint32_t tmr = 0;

    if (millis() - tmr >= 5) {
        tmr = millis();
        static uint8_t i = 0;

        setAnod(i);
        setDigit(buf[i]);
        if (++i >= 4)i = 0;
    }
}
*/
#pragma endregion
//-----func-----//

void setup() {
#if debugMode == 1
    Serial.begin(9600);
#endif

    TCCR2B = (TCCR2B & B11111000) | 1;  //timer2 (~7kHz)
    bitSet(TCCR2A, WGM21);
    bitSet(TIMSK2, OCIE2A);     //прерывание по таймеру

    for (uint8_t i = 0;i < 11;i++)pinMode(i < 7 ? segments[i] : anod[i - 7], OUTPUT);
}

void loop() {
    static uint32_t tmr = 0;
    if (millis() - tmr >= 100) {
        tmr = millis();
        static uint16_t i = 0;
        sptlit(i);
        if (++i >= 10000)i = 0;
    }
}
