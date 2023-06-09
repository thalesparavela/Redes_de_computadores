#include "Thales_SPI_Config.h"

void blink_led_on() {
  set_bank(0x02);
  write_register(OPWCR, MIREGADR, PHLCON);
  write_register(OPWCR, MIWRL, 0b10000000);
  write_register(OPWCR, MIWRH, 0b00001000);
}

void blink_led_off() {
  set_bank(0x02);
  write_register(OPWCR, MIREGADR, PHLCON);
  
  write_register(OPWCR, MIWRL, 0b10010000);
  write_register(OPWCR, MIWRH, 0b00001001);
}

void setup() {
  Serial.begin(9600);
  config_spi();

}

void loop() {
delay(1000);
blink_led_on();
delay(1000);
reset_enc();
}
