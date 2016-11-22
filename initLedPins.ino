void setLedStates(void) {
  if (myAddress64 == leaderAddress64) {
    digitalWrite(PIN_BLUE_LED, HIGH);
    digitalWrite(PIN_GREEN_LED, LOW);
    digitalWrite(PIN_RED_LED, LOW);
  } else {
    digitalWrite(PIN_BLUE_LED, LOW);
    if (isInfected) {
      digitalWrite(PIN_GREEN_LED, LOW);
      digitalWrite(PIN_RED_LED, HIGH);
    } else {
      digitalWrite(PIN_GREEN_LED, HIGH);
      digitalWrite(PIN_RED_LED, LOW);
    }
  }
}
