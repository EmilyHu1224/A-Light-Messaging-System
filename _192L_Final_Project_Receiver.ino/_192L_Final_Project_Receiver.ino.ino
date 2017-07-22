/*
 * configuration
 * 
 * interruptPin = 2
 * 
 */

/*
 * To do
 * 
 * Remove the brackets around messages
 * 
 * 
 */

 /*
 * Test cases
 * 
 * "hello world":[.... . .-.. .-.. ---   .-- --- .-. .-.. -..]
 * 
 * "911": [----. .---- .---- ]
 * 
 */

const int inputPins[] = {8, 9, 10, 11};

const int delayTime = 30;

// Timer config
const unsigned long period = 65535;
volatile long cycle;


volatile bool pinsTriggered[] = {false, false, false, false};

String morseCode = "";

void setup()
{
    Serial.begin(9600);

    pinMode(inputPins[0], INPUT);
    pinMode(inputPins[1], INPUT);
    pinMode(inputPins[2], INPUT);
    pinMode(inputPins[3], INPUT);

    setupTimer();

    attachInterrupt(0, handler, CHANGE);
}

void setupTimer()
{
    cli();
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B = (1 << 2);
    TIMSK1 |= (7 << 0);
    OCR1A = -1;
    OCR1B = period - 1;
    sei();   
}

ISR (TIMER1_COMPB_vect)
{
    attachInterrupt(0, handler, CHANGE);
    TCNT1 = 0;
    cycle++;
}

void loop()
{    
    if (pinsTriggered[3])
    {
        if (!pinsTriggered[0])
        {
            // end of message
            // translate the message from morse code to text message
            String message = decryptFromMorseCode(morseCode);
            
            // print out the decrypted message
            Serial.println(getTime() + ":");
            Serial.println("      [" + message + "]");
            Serial.println("");

            morseCode = "";
        }
        else
        {
            //read morse code
            morseCode += readMorseCode();
        }

        pinsClear();
    }
}


String getTime()
{
    unsigned long microseconds = (cycle * period + TCNT1) * (256.0/16.0);
    unsigned long seconds = microseconds / 1000000;
    unsigned long minutes = seconds / 60;
    seconds -= minutes * 60;

    String timeString = "";

    if (minutes < 10) timeString += '0';
    timeString += (String)minutes + ':';

    if (seconds < 10) timeString += '0';
    timeString += (String)seconds;
    
    return timeString;
}

char readMorseCode()
{
    if (pinsTriggered[1] && !pinsTriggered[2])
    {
        //10
        return '.';
    }
    if (pinsTriggered[1] && pinsTriggered[2])
    {
        //11
        return '-';
    }
    if (!pinsTriggered[1] && !pinsTriggered[2])
    {
        //00
        return ' ';
    }

    return '!';
}

void handler()
{
    updatePinsTriggered();
}

void updatePinsTriggered()
{
    for (int i = 0; i < 4; i++)
    {
        if (digitalRead(inputPins[i]))
        {
            pinsTriggered[i] = true;
            return;
        }
    }
    
    //Serial.println("Unexpected input pin:(");
}

void pinsClear()
{
    for (int i = 0; i < 4; i++)
    {
        pinsTriggered[i] = false;
    }
}

String decryptFromMorseCode(String morseCode)
{
    String message = "";
    
    for (int i = 0; i < morseCode.length(); i++)
    {
        String sequence = getNextSequence(morseCode, i);

        message += (String)morseToText(sequence);
    }

    return message;
}

String getNextSequence(String morseCode, int& i)
{
    if (morseCode[i] == ' ' && i < morseCode.length() - 1 && morseCode[i+1] == ' ')
    {
        i++;
        return " ";
    }

    String sequence = "";
    
    for (; i < morseCode.length(); i++)
    {
        if (morseCode[i] != ' ')
        {
            sequence += morseCode[i];
        }
        else
        {
            break;
        }
    }

    return sequence;
}

char morseToText(String sequence)
{
    if (sequence.length() == 1 && sequence[0] == ' ') return ' ';

    char c;
    
    if (sequence.length() == 5) //number
    {
        c = morseToNumber(sequence);
    }
    else
    {
        c = morseToLetter(sequence);
    }
    
    return c;
}

char morseToNumber(String sequence)
{
    const String numbersMorseCode[] = {"-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."};
    
    for (int i = 0; i < 10; i++)
    {
        if (sequence == numbersMorseCode[i])
        {
            return '0' + i;
        }
    }

    Serial.println("Unexpected number morse code: [" + sequence + "]");
    return 0;
}

char morseToLetter(String sequence)
{
    const String lettersMorseCode[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", // a-i
                                       ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // j-r
                                       "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."}; //s-z
    for (int i = 0; i < 26; i++)
    {
        if (sequence == lettersMorseCode[i])
        {
            return 'a' + i;
        }
    }

    Serial.println("Unexpected letter morse code: [" + sequence + "]");
    return 0;
}
