#define PRODUCTION_MODE true

const int PHBTN_1 = 5;
const int PHBTN_2 = 16; 
const int WAVEFORM = 33;
const int ALTN_WAVEFORM = 15;
const int FIFTY = 50;

bool dataEnabled = false;
bool altMode = false;

void setup() {
    pinMode(PHBTN_1, INPUT_PULLDOWN);
    pinMode(PHBTN_2, INPUT_PULLDOWN);
    pinMode(WAVEFORM, OUTPUT);
    pinMode(ALTN_WAVEFORM, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(PHBTN_1), toggleData, RISING);
    attachInterrupt(digitalPinToInterrupt(PHBTN_2), toggleMode, RISING);
}
void generatePHBTN_1() {
    int p = 1100;  
    int e = 500;
    int n = 17;
    int u = 3000;
    int sync_on = 50;
    if(altMode){
      e=250;
      u=1500;
    }

    if (!PRODUCTION_MODE) { 
        p *= 1000;
        e *= 1000;
        u *= 1000;
        sync_on *= 1000;
    }

    for (int i = 0; i < n; i++) {
        digitalWrite(WAVEFORM, HIGH);
        delayMicroseconds(p + (i * FIFTY));
        digitalWrite(WAVEFORM, LOW);
        delayMicroseconds(e);
    }
  
    delayMicroseconds(u);
    digitalWrite(ALTN_WAVEFORM, HIGH);
    delayMicroseconds(sync_on);
    digitalWrite(ALTN_WAVEFORM, LOW);
}

void loop() {
    if (dataEnabled) {
        generatePHBTN_1();
    } else {
        digitalWrite(WAVEFORM, LOW);
        digitalWrite(ALTN_WAVEFORM, LOW);
    }
}

void toggleData() {
    dataEnabled = !dataEnabled;  
}

void toggleMode() {
    altMode = !altMode;
}


