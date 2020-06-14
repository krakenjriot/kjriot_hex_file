#include <kjr3wire.h>
 /***
 *      _  ______      _    _  _______ _   _       _ ____      ___ ___ _____
 *     | |/ /  _ \    / \  | |/ / ____| \ | |     | |  _ \    |_ _/ _ \_   _|
 *     | ' /| |_) |  / _ \ | ' /|  _| |  \| |  _  | | |_) |    | | | | || |
 *     | . \|  _ <  / ___ \| . \| |___| |\  | | |_| |  _ < _   | | |_| || |
 *     |_|\_\_| \_\/_/   \_\_|\_\_____|_| \_|  \___/|_| \_(_) |___\___/ |_|
 *
 *                          KJR 3 WIRE PROTOCOL
 *                       https://krakenjriot.tech/
 *                  https://internetofthingsbuilder.com/
 *                          funsideofwine@gmail.com
 *
 * MIT License
 *
 * Copyright (c) 2020 Rolly Falco Villacacan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

  #include <avr/wdt.h>
  #include <avr/boot.h>
  #define SIGNAL_LINE 6 //6
  #define MASTER_LINE 7 //7
  #define SLAVE_LINE  8 //8
  #define LINE_ID '2'
  #define SERIAL_SIZE 300
  
  const char * analog_input_data;
  const char * digital_input_data;
  const char * health_data;
 

  char concatenated_chars[SERIAL_SIZE]; //
  char cached_str[SERIAL_SIZE];
  char id_char_arr[64]; //serial number


  bool init_t = true; // first statement get executed only once
  bool startRecording = false;
  bool itsforme = false;
  bool goto_next = false;
  bool SIGNAL_LINE_STAT = false;

  unsigned long ii = 0;

  //***************************** setup ********************************
  //***************************** setup ********************************
  //***************************** setup ********************************
  //***************************** setup ********************************
  //
  void setup()
  {
  //********************************************************************
    //wdt_enable(WDTO_8S);
  //********************************************************************

  //********************************************************************
    Serial.begin(9600);
    //Serial.begin(115200);
    while (!Serial)
    {
      ; // wait for serial port to initialize
    }
  //********************************************************************
    pinMode(SIGNAL_LINE, INPUT);
    pinMode(MASTER_LINE, INPUT);
    pinMode(SLAVE_LINE, OUTPUT);
    pinMode(42, OUTPUT);
  //********************************************************************
//INIT STATEMENT
//********************************************************************
//********************************************************************

          String id;

        #define SIGRD 5
        #if defined(SIGRD) || defined(RSIG)
        for (uint8_t i = 0; i < 5; i += 2)
    {
      id += boot_signature_byte_get(i);
        }
        for (uint8_t i = 14; i < 24; i += 1)
    {
      id += boot_signature_byte_get(i);
        }
        #endif

        byte x = 0;
        while (id[x] != '\0')
        {
            id_char_arr[x] = id[x];
            x++;
        }

        id_char_arr[x] = '\0'; //terminate string


        Serial.println(F("initialize(success!)"));
        Serial.print(F("hardware id: "));
        Serial.println((id_char_arr));

//********************************************************************
//********************************************************************
  //********************************************************************
  }//end setup
  //***************************** setup ********************************
  //***************************** setup ********************************
  //***************************** setup ********************************



  //***************************** loop ********************************
  //***************************** loop ********************************
  //***************************** loop ********************************
  void loop()
  {
    //wdt_reset();
    //********************************************************************
    char od_pins_slave[70];
    //********************************************************************



    //TIMER TRIGGER
    //********************************************************************
    SIGNAL_LINE_STAT = digitalRead(SIGNAL_LINE);
    //********************************************************************

    //********************************************************************
    if(!SIGNAL_LINE_STAT)
    {
      if(itsforme)
      {
        if(init_t){
          sprintf(concatenated_chars,"%c0,%s,",LINE_ID, id_char_arr);
          encode_lapse(concatenated_chars, SLAVE_LINE);
          init_t = false;
        } else {
          concatenated_chars[0] = '\0'; //erase content
          analog_input_data = "123.45*123.45*123.45*123.45*123.45*123.45*123.45*123.45*123.45*123.45*123.45*123.45*123.45*123.45*123.45*123.45"; //16 sensors serial data (input analog)
          digital_input_data = "0000000000000000000000000000000000000000000000000000000000000000000000"; //70 pins (input digital)          
          health_data = "200.50,212.89"; // temperature and humidity
          //chars 190 max
          sprintf(concatenated_chars,"%c,%s,%s,%s,%s,", LINE_ID, health_data, digital_input_data, od_pins_slave, analog_input_data);
          encode_lapse(concatenated_chars , SLAVE_LINE);
          //wdt_reset();
          itsforme = false;
        }//init
          //Serial.print("concatenated_chars: ");
          //Serial.println(concatenated_chars);        
      }//itsforme
    }//if
    //********************************************************************

    //********************************************************************
    //THEN HERE!
    if(SIGNAL_LINE_STAT)
    {

      unsigned long lapse = pulseIn(MASTER_LINE, HIGH);
      char chx = decode_lapse(lapse);
      //Serial.println(chx);
      if(chx == '{')
      {
         startRecording = true;
      }

      if(startRecording)
      {
        cached_str[ii] = chx;
        ii++;
      }

      uint8_t c = 0, d = 0;
      unsigned long i = 0;
      if(chx == '}')
      {
        cached_str[ii] = '\0'; //terminated the string char array

        c = 0;
        for(i=0;cached_str[i];i++)
        {
          if(cached_str[i]=='{')
          {
            c++;
          }
        }

        d = 0;
        for(i=0;cached_str[i];i++)
        {
          if(cached_str[i]=='E')
          {
            d++;
          }
        }

        if(c == 1 && d == 0)
        {
          if(cached_str[1] == LINE_ID){
            itsforme = true;
            goto_next = true;
            pinMode(SLAVE_LINE, OUTPUT);
          }else{
            itsforme = false;
            pinMode(SLAVE_LINE, INPUT);
            //goto_next = false;
          }

        }

        startRecording = false;
        ii = 0;
      }//chx == '}'
      //wdt_reset();
    }//if
    //********************************************************************

    //********************************************************************
    //THEN HERE AGAIN!
    if(goto_next)
    {
      //you code here...
      //you code here...
      //you code here...
      //you code here...
      //you code here...
      //you code here...
      //you code here...
      //************************************************************
      //EXTRACT OUTPUT DIGITAL STRING FROM PinStat STRING
      unsigned int i = 74, xx = 0;
      
      while(cached_str[i] != ','){
        od_pins_slave[xx] = cached_str[i];
        xx++;
        i++;
      }
        od_pins_slave[xx] = '\0';
      //************************************************************
      //sample code
      digitalWrite(42, od_pins_slave[42]=='0'?false:true);


      Serial.print(F("serial_data_rcvd_from_master: "));
      Serial.println(cached_str);

      Serial.print("od_pins_slave: ");
      Serial.println(od_pins_slave);
      //count cycles
      //count_cycle++;
      //Serial.print(F("Cycle Counter: "));
      //Serial.println(count_cycle);

      //do not remove!
      cached_str[0] = '\0'; // erase content
      goto_next = false;
      //wdt_reset();
    }

  }//loop
  //***************************** loop ********************************
  //***************************** loop ********************************
  //***************************** loop ********************************
