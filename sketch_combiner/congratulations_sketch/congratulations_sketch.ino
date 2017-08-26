void setup() {
Serial.begin();
}
void loop() {
  Serial.println("Congratulations, you have installed the STM32duino bootloader");
  Serial.println("See https://github.com/rogerclarkmelbourne/STM32duino-bootloader\n\n");
  Serial.println("For more information about Arduino on STM32");
  Serial.println("and http://www.stm32duino.com\n\n\n");
  delay(1000);
}
