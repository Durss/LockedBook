int coil_1 = 3;
int coil_2 = 5;
int offset = 4;
int step = 0;
boolean opening = false;
boolean closing = false;
  
void setupStepper()
{
  Serial.begin(9600);
  pinMode(coil_1, OUTPUT);  
  pinMode(coil_1+offset, OUTPUT);
  pinMode(coil_2, OUTPUT);  
  pinMode(coil_2+offset, OUTPUT);
    //pinMode(A0, INPUT);
  }

void coilForward(int pin)
{
  digitalWrite(pin, HIGH);
  digitalWrite(pin+offset, LOW);
}

void coilBackward(int pin)
{
  digitalWrite(pin, LOW);
  digitalWrite(pin+offset, HIGH);  
}

void coilOff(int pin)
{
  digitalWrite(pin, LOW);
  digitalWrite(pin+offset, LOW);
}


void openBook() {
  Serial.println("open !");
  opening = true;
  for(int i = 0 ; i < 50; i++) loopStepper();
  opening = false;
}


void closeBook() {
  Serial.println("close !");
  closing = true;
  for(int i = 0 ; i < 50; i++) loopStepper();
  closing = false;
}

void loopStepper()
{
  if(!opening && !closing) return;
  
  step += opening? 1 : -1;
  step = step % 8;
  if(step < 0) step = 8 + step;
  
  switch(step)
  {
    case 0:
      coilForward(coil_1);
      coilOff(coil_2);
      break;
      
    case 1:
      coilForward(coil_1);
      coilForward(coil_2);
      break;
      
    case 2:
      coilOff(coil_1);
      coilForward(coil_2);
      break;
    
    case 3:
      coilBackward(coil_1);
      coilForward(coil_2);
      break;
      
    case 4:
      coilBackward(coil_1);
      coilOff(coil_2);
      break;
      
    case 5:
      coilBackward(coil_1);
      coilBackward(coil_2);
      break;
      
    case 6:
      coilOff(coil_1);
      coilBackward(coil_2);
      break;
    
    case 7:
      coilForward(coil_1);
      coilBackward(coil_2);
  }
  
  delay(5);
}
