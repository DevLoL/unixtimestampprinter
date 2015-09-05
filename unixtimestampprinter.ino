// Unix Timestamp Printer
// (C) 2015 by Thomas R. Koll
// MIT public license

/*
 * The magnificent Unix Timestamp Printer will print unique timestamps for your private use.
 *
 * Inspration and further readings:
 *   http://elektronik-kompendium.de/public/arnerossius/bastel/m31a.htm
 *   how to control a small Epson printer from a table calc with an AVR µC
 *
 */

//#include <Wire.h>
//#include <DS1307new.h>

const bool debug = true;
// pins for the printer
const int printer_magnet = 4;
const int printer_motor = 3;
const int printer_char_start = 5;
const int printer_char_end = 7;
const int printer_char_reset = 6;
static int printer_position;
const int printer_motor_speed = 64;

const char printer_chars[] = "#123456789-,.";
// second row is " +x:#*ENMC= -%"

// print button
const int print_timestamp = 8;
static int time_since_print = 0;
const int time_between_prints = 5000; // milliseconds

// timekeeping
static unsigned int seconds = 1441121804; // roughly

void setup() {
  pinMode(printer_magnet, OUTPUT);
  pinMode(printer_motor, OUTPUT);
  analogWrite(printer_motor, 255);
  pinMode(printer_char_start, INPUT);
  pinMode(printer_char_end, INPUT);
  pinMode(printer_char_reset, INPUT);
  pinMode(print_timestamp, INPUT);


  analogWrite(printer_motor, 0);
  digitalWrite(printer_magnet, HIGH);
  delay(300);
  analogWrite(printer_motor, 255);
  digitalWrite(printer_magnet, LOW);
  Serial.begin(9600);
  
  delay(200);
  //printTimeStamp();
}

// map a string into an array of positions on the printer wheel
int *printerPositions(char str[]) {
  int len = strlen(str);
  int result[len];
  for(int i=0; i < len; i++) {
    result[i] = strchr(printer_chars, str[i]) - printer_chars;
  }
  return result;
}

void printTimeStamp() {
  Serial.println("Printing...");
  // return if pressed recently
  if (time_since_print > millis()) {
    return;
  }
  time_since_print = millis() + time_between_prints;
  char str_seconds[18];
  sprintf(str_seconds, "%d", seconds);
  int *string_positions;
  string_positions = printerPositions(str_seconds);

  analogWrite(printer_motor, printer_motor_speed);
  static int printer_position = 0;

  printerSeekStart();
  // print from left to right
  if(debug) {
    Serial.println(sizeof(string_positions));
  }
  for(int i = sizeof(string_positions); i > 0; i--) {
    printChar(string_positions[i]);
  }

  // for a line break we need to wait for a little longer before we release the magnet
  printerSeekNextChar();
  printerSeekNextChar();
  digitalWrite(printer_magnet, LOW);

  analogWrite(printer_motor, 255);
  
  Serial.print('Printed timestamp r');
  Serial.println(seconds);
}

void printerSeekStart() {
  if(debug) {
    Serial.println("Seeking first char");
  }
  while(digitalRead(printer_char_reset) == LOW) {
    digitalWrite(printer_magnet, !digitalRead(printer_char_end));
    delay(1);
    // bring the printer motor to 0 position
  }
}

void printerSeekNextChar() {
  while (digitalRead(printer_char_start) == LOW) {
    delay(1);
  }
  printer_position += 1;
  if (digitalRead(printer_char_reset) == HIGH) {
    printer_position = 0;
  }
}

void printChar(int position) {
  digitalWrite(printer_magnet, LOW);
  do {
    printerSeekNextChar();
  } while(position != printer_position);

  // actually print the char
  do {
    digitalWrite(printer_magnet, HIGH);
    delay(1);
  } while(printer_char_end == LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(print_timestamp) == HIGH) {
    printTimeStamp();
  }
  // read unixtimestamp from serial and update in our system
  if(Serial.available() > 0) {
    String new_seconds = Serial.readString();
    if(new_seconds.toInt() > 100) {
      seconds = new_seconds.toInt();
      Serial.print("Time changed to ");
      Serial.println(seconds);
    } else if (new_seconds == "p") {
      printTimeStamp();
    }
    Serial.flush();
  }
}
