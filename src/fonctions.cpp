#include <lib.h>

void Blink(void){
    digitalWrite(LEDU1, HIGH); 
    digitalWrite(LEDU2, HIGH);  
    delay(500);             
    digitalWrite(LEDU1, LOW); 
    digitalWrite(LEDU2, LOW);  
    delay(500);             
}