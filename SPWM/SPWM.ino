boolean positive_amp=true;
int x=0;
#define ARRAY_LENGTH 101
#define Total_time 16175
#define Frequency 50
int sinus[]={0,8,16,23,31,39,47,55,63,71,78,86,93,101,108,115,122,129,
136,143,149,156,162,168,174,180,185,191,196,201,206,210,215,219,223,
227,230,234,237,239,242,244,246,248,250,251,252,253,254,254,255,254,
254,253,252,251,250,248,246,244,242,239,237,234,230,227,223,219,215,
210,206,201,196,191,185,180,174,168,162,156,149,143,136,129,122,115,
108,101,93,86,78,71,63,55,47,39,31,23,16,8,0};
float adjust_fact=((1.0/Frequency/2.0)/Total_time)*1000000;
boolean output=false;
void setup() {
 pinMode(9,OUTPUT); 
 pinMode(10,OUTPUT);
}

void loop() {
  if(x==ARRAY_LENGTH){
    positive_amp=!positive_amp;
    output=false;
  }

  int out_value=output?HIGH:LOW;
  if(positive_amp)
      digitalWrite(9,out_value);
  else 
      digitalWrite(10,out_value);
  delayMicroseconds(sinus[x]*adjust_fact);
  x++;
  
    

}
