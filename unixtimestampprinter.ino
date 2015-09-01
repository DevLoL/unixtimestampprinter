// Unix Timestamp Printer
// (C) 2015 by Thomas R. Koll
// MIT public license

/*
 * The magnificent Unix Timestamp Printer will print unique timestamps for your private use.
 */

// pins for the printer
const int printer_magnet = 3;
const int printer_motor = 4;
const int printer_char_start = 5;
const int printer_char_end = 6;
const int printer_char_reset = 7;

// print button, debounced
const int print_timestamp = 8;
const int time_since_print = 0;
const int time_between_prints = 5000; // milliseconds

void setup() {
  pinMode(printer_magnet, OUTPUT);
  pinMode(printer_motor, OUTPUT);
  pinMode(printer_char_start, INPUT);
  pinMode(printer_char_end, INPUT);
  pinMode(printer_char_reset, INPUT);
  pinMode(print_timestamp, INPUT);

}

void printTimeStamp() {
  // return if pressed recently
  if(time_since_print > millis()) {
    return;
  }
  time_since_print = millis() + time_between_prints;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(print_timestamp) == HIGH) {
    printTimeStamp();
  }
}
