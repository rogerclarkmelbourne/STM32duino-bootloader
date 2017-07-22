void setup() {
Serial.begin();
}
void loop() {
  Serial.println("\n\n\n");
  Serial.println("Congratulations, you have installed the STM32duino bootloader");
  Serial.println("For more information about Arduino on STM32"); 
  Serial.println("See https://github.com/rogerclarkmelbourne/STM32duino-bootloader");
  Serial.println("and http://www.stm32duino.com");
  delay(1000);
}
