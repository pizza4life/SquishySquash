//Code for Squash Scoreboard Transmitter for Nobles
//Wyatt Ellison, November, 2016

//setting integers for pin numbers
const int HomeScoreUpPin;
const int HomeScoreDownPin;
const int HomeGameUpPin;
const int HomeGameDownPin;
const int GuestScoreUpPin;
const int GuestScoreDownPin;
const int GuestGameUpPin;
const int GuestGameDownPin;
const int ResetButtonPin;
//setting variables for i/o button values
int HomeScoreUp = 0;
int HomeScoreDown = 0;
int HomeGameUp = 0;
int HomeGameDown = 0;
int GuestScoreUp = 0;
int GuestScoreDown = 0;
int GuestGameUp = 0;
int GuestGameDown = 0;
int ResetButton = 0;

void setup() {
//defining pins as inputs
pinMode(HomeScoreUpPin, INPUT);
pinMode(HomeScoreDownPin, INPUT);
pinMode(HomeGameUpPin, INPUT);
pinMode(HomeGameDownPin, INPUT);
pinMode(GuestScoreUpPin, INPUT);
pinMode(GuestScoreDownPin, INPUT);
pinMode(GuestGameUpPin, INPUT);
pinMode(GuestGameDownPin, INPUT);
pinMode(ResetButtonPin, INPUT);

}

void loop() {
 //constantly setting pins values to int values to be tested
HomeScoreUp = digitalRead(HomeScoreUpPin);
HomeScoreDown = digitalRead(HomeScoreDownPin);
HomeGameUp = digitalRead(HomeSGameUpPin);
HomeGameDown = digitalRead(HomeGameDownPin);
GuestScoreUp = digitalRead(GuestScoreUpPin);
GuestScoreDown = digitalRead(GuestScoreDownPin);
GuestGameUp = digitalRead(GuestGameUpPin);
GuestGameDown = digitalRead(GuestGameDownPin);
ResetButton = digitalRead(ResetButtonPin);

//testing for button-presses
if(HomeScoreUp == HIGH){
  
  }

if(HomeScoreDown == HIGH){
  

  }

if(HomeGameUp == HIGH){
  
  
  }
if(HomeGameDown == HIGH){
  
  
  }
if(GuestScoreUp == HIGH){
  
  }

if(GuestScoreDown == HIGH){
  
  
 }
if(GuestGameUp == HIGH){


  }
if(GuestGameDown == HIGH){
  

  }
if(ResetButton == HIGH){

    
  }
  
}
