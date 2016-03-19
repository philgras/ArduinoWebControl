int redPin = 11;
int bluePin = 10;
int greenPin = 9;


void setup(){
  Serial.begin(9600);
  
}

void setColor(int red, int green, int blue){
 
 
  analogWrite(redPin, red);
  analogWrite(bluePin, blue);
  analogWrite(greenPin, green);
  
}

int readNumber(){

String inStr = "";
int i;
  
while(1){

  if(Serial.available() >0){
    
    i = Serial.read();
    if(i == ';'){
      return inStr.toInt();
    }
    inStr += (char) i;
  }

}



}

void loop(){

  int r,g,b;
  
  r = readNumber();
  g = readNumber();
  b = readNumber();
  Serial.println(r,DEC);
  Serial.println(g,DEC);
  Serial.println(b,DEC);
  setColor(r,g,b);
 
 
 
  
}
