// caps?
// special characters + spaces?
//12:59AM

/*
 * configuration
 * 
 * interruptPin = 2
 * 
 * first triggerPin = 0 (photoresistorPins[0])
 */

const int ledPins[] = {8, 9, 10, 11};

const int delayTime = 30;

int triggerPin = 0;
int pins[4] = {0, 1, 2, 3};

void setup()
{
    Serial.begin(9600);

    pinMode(ledPins[0], OUTPUT);
    pinMode(ledPins[1], OUTPUT);
    pinMode(ledPins[2], OUTPUT);
    pinMode(ledPins[3], OUTPUT);
}

void loop()
{
    if (Serial.available())
    {
        // Read in the message from Serial Monitor
        String message = Serial.readString();
        Serial.println("Message: [" + message + ']');

        // Translate the message to morse code
        String morseCode = toMorseCode(message);
        Serial.println("Morse code: [" + morseCode + ']');

        // Send the morse code forward through led
        sendMorseCode(morseCode);
    }
}

const String lettersMorseCode[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", // a-i
".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // j-r
"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."}; //s-z

const String numbersMorseCode[] = {"-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."};

String toMorseCode(String message)
{
    String morseCode = "";
    
    for (int i = 0; i < message.length(); i++)
    {        
        if (isNumber(message[i]))
        {
            morseCode += numbersMorseCode[message[i] - '0'];
        }
        else if (isLetter(message[i]))
        {
            morseCode += lettersMorseCode[message[i] - 'a'];
        }
        else if (message[i] == ' ')
        {
            morseCode += " ";
        }
        else
        {
            Serial.println("Unexpected character: [" + message[i] + ']');
        }

        morseCode += " ";
    }
    
    return morseCode;
}

void sendMorseCode(String morseCode)
{
    for (int i = 0; i < morseCode.length(); i++)
    {
        startSignal();
      
        switch(morseCode[i])
        {
            case '.':
              dot();
              break;
            case '-':
              dash();
              break;
            case ' ':
              gap();
              break;
            default:
              Serial.println("!!!Invalid morse code: [" + (String)morseCode[i] + "]");
        }

        endSignal();
    }

    endSignal();  
}

void startSignal()
{
    digitalWrite(ledPins[0], HIGH);
    delay(delayTime);
    digitalWrite(ledPins[0], LOW);
    delay(delayTime);
}

void endSignal()
{
    digitalWrite(ledPins[3], HIGH);
    delay(delayTime);
    digitalWrite(ledPins[3], LOW);
    delay(delayTime);
}

void dot()
{
    //10
    digitalWrite(ledPins[1], HIGH);
    delay(delayTime);
    digitalWrite(ledPins[1], LOW);
    delay(delayTime);
    
    Serial.print('.');
}

void dash()
{
    //11
    digitalWrite(ledPins[1], HIGH);
    delay(delayTime);
    digitalWrite(ledPins[1], LOW);

    digitalWrite(ledPins[2], HIGH);
    delay(delayTime);
    digitalWrite(ledPins[2], LOW);
    
    Serial.print('-');
}

void gap()
{
    //00
    delay(delayTime);
    delay(delayTime);
    
    Serial.print(' ');
}


bool isNumber(char c)
{
    return c >= '0' && c <= '9';
}

bool isLetter(char c)
{
    return c >= 'a' && c <= 'z';
}

