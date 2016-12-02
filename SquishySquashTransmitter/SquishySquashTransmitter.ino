//Code for Squash Scoreboard Transmitter for Nobles
//Wyatt Ellison, Gustave Ducrest, Dominic Manzo, November, 2016

byte remote_freq = 120;
byte remote_chan = 5;
byte spi_status = 0;
byte payload_value = 0;
#define payload_width 10
byte payload[payload_width];
#define MISO A5
#define MOSI A4
#define SCK A3
#define CSN A2
#define CE A1

#define HomeScoreUpPin 2
#define HomeScoreDownPin 3
#define HomeGameUpPin 4
#define HomeGameDownPin 5
#define GuestScoreUpPin 6
#define GuestScoreDownPin 7
#define GuestGameUpPin 8
#define GuestGameDownPin 9
#define ResetButtonPin 10

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

void spi_write(byte data) {
  byte counter;
  byte bit_mask[8] = {128,64,32,16, 8,4,2,1};
  byte miso_bit[8];
  spi_status = 0;
  
  digitalWrite(SCK, 0);
  digitalWrite(MOSI, 0);
  
  for(counter = 0; counter < 8; counter++) {
    digitalWrite(SCK, 0);
    if (data & bit_mask[counter])
      digitalWrite(MOSI, 1);
    else digitalWrite(MOSI, 0);
    
    digitalWrite(SCK, 1);
    if (digitalRead(MISO) == 1) {
      miso_bit[counter] = 1;
    }
    else {
      miso_bit[counter] = 0;
    }
  
    digitalWrite(SCK, 1);
  
    spi_status += (miso_bit[counter] * bit_mask[counter]);
    digitalWrite(SCK, 0);
  }
}

void write_reg(byte reg, byte data) {
  byte counter = 0;
  
  digitalWrite(CSN, 0);
  
  if ((reg == 42) || (reg == 48)) { // reg 10 = RX_ADDR_P0, reg 16 = TX_ADDR
    spi_write(reg);
    for (counter = 0; counter < 5; counter++) {
      spi_write(data); // TX and RX address is remote_chan repeated 5 times
    }
  }
  
  else if (reg == 97) { // command 97 (0x61) = RD_RX_PLOAD, read received payload
    spi_write(reg);
    for (counter = 0; counter < data; counter++) { // data is payload_width
      spi_write(255);
      payload[counter] = spi_status;
    }
  }
  
  else if (reg == 160) { // command 160 (0xA0) = WR_TX_PLOAD, write transfer payload
    spi_write(reg);
    for (counter = 0; counter < data; counter++) { // data is payload_width
      spi_write(payload[counter]);
    }
  } 
  
  else {
    spi_write(reg);   
    spi_write(data);
  }
    
  digitalWrite(CSN, 1);
}

void flush_buf(void) {
  digitalWrite(CSN, 0);
  spi_write(225);   // FLUSH_TX
  
  spi_write(226);   // FLUSH_RX
  digitalWrite(CSN, 1); 
}

void rx_mode(void) {
  digitalWrite(CE, 0);
  
  write_reg(32, 15);           // CONFIG: 2 byte CRC, power up, receiver
  write_reg(33, 1);            // EN_AA: enable auto ack. pipe0
  write_reg(34, 1);            // EN_RXADDR: enable pipe0
  write_reg(35, 3);            // SETUP_AW: setup address width of 5 bytes
  write_reg(36, 3);            // SETUP_RETR: default
  write_reg(37, remote_freq);  // RF_CH: select frequency
  write_reg(38, 7);         // RF_SETUP: TX_PWR = 0dBm, datarate = 1Mbps
  write_reg(39, 112);     // STATUS: clear all IRQ flags
  write_reg(42, remote_chan); // RX_ADDR_P0: write RX address
  write_reg(48, remote_chan); // TX_ADDR: writes TX_Address
  write_reg(49, payload_width);  // RX_PW_P0: set RX payload width
  
  //flush_buf();
  
  digitalWrite(CE, 1);        // Set CE pin high to enable device
}

void tx_mode(void) {
  digitalWrite(CE, 0); 
  
  write_reg(48, remote_chan); // TX_ADDR: writes TX_Address
  write_reg(42, remote_chan); // RX_ADDR_P0: same as TX_Address
  write_reg(33, 1);            // EN_AA: enable auto ack. pipe0
  write_reg(34, 1);            // EN_RXADDR: enable pipe0
  write_reg(35, 3);            // SETUP_AW: setup address width of 5 bytes
  write_reg(36, 3);           // SETUP_RETR: default
  write_reg(37, remote_freq);  // RF_CH: select frequency
  write_reg(38, 7);         // RF_SETUP: TX_PWR = 0dBm, datarate = 1Mbps
  write_reg(39, 112);     // STATUS: clear all IRQ flags
  write_reg(32, 14);           // CONFIG: 2 byte CRC, power up, transfer
  write_reg(49, payload_width); // WR_TX_PLOAD: set TX payload width
  
  //flush_buf();
  
  digitalWrite(CE, 0);        // Set CE pin high to enable device
}

void rf_init(void) {
  write_reg(32, 14);    // CONFIG: 2 byte CRC, power up, transmitter
  write_reg(33, 63);    // EN_AA: Default, Auto ack all pipe's
  write_reg(34, 3);     // EN_RXADDR: Default, pipe 0 & 1 enabled
  write_reg(35, 3);     // SETUP_AW: Default, 5 byte address
  write_reg(39, 112);     // STATUS: clear all IRQ flags
  
  flush_buf();
}

void setup() {
pinMode(MISO, INPUT);  // MISO
pinMode(MOSI, OUTPUT); // MOSI
pinMode(SCK, OUTPUT); // SCK
pinMode(CSN, OUTPUT); // CSN
pinMode(CE, OUTPUT); // CE

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

digitalWrite(CSN, 1);
digitalWrite(SCK, 0);
digitalWrite(CE, 0);

rf_init();
   
delay(300);
digitalWrite(CSN, 1);
digitalWrite(CE, 0);
delay(100);

Serial.begin(9600);

tx_mode();
}

void loop() {

write_reg(7,0); // read value of STATUS reg

byte counter;
byte tx_trigger = 0;

 //constantly setting pins values to int values to be tested
HomeScoreUp = digitalRead(HomeScoreUpPin);
HomeScoreDown = digitalRead(HomeScoreDownPin);
HomeGameUp = digitalRead(HomeGameUpPin);
HomeGameDown = digitalRead(HomeGameDownPin);
GuestScoreUp = digitalRead(GuestScoreUpPin);
GuestScoreDown = digitalRead(GuestScoreDownPin);
GuestGameUp = digitalRead(GuestGameUpPin);
GuestGameDown = digitalRead(GuestGameDownPin);
ResetButton = digitalRead(ResetButtonPin);

// reset payload to no buttons pressed
for (counter = 0; counter < payload_width; counter++) {
      payload[counter] = 0;
}

counter = 0;

//testing for button-presses
if(HomeScoreUp == HIGH) {
  Serial.println('HomeScoreUp pressed');
  payload[counter] = 1;
  tx_trigger = 1;
}

counter++;

if(HomeScoreDown == HIGH){
  Serial.println('HomeScoreDown pressed');
  payload[counter] = 1;
  tx_trigger = 1;
}

counter++;

if(HomeGameUp == HIGH){
  Serial.println('HomeGameUp pressed');
  payload[counter] = 1;
  tx_trigger = 1;
}

counter++;

if(HomeGameDown == HIGH){
  Serial.println('HomeGameDown pressed');
  payload[counter] = 1;
  tx_trigger = 1;
}

counter++;

if(GuestScoreUp == HIGH){
  Serial.println('GuestScoreUp pressed');
  payload[counter] = 1;
  tx_trigger = 1;
}

counter++;

if(GuestScoreDown == HIGH){
  Serial.println('GuestScoreDown pressed');
  payload[counter] = 1;
  tx_trigger = 1;
}

counter++;

if(GuestGameUp == HIGH){
  Serial.println('GuestGameUp pressed');
  payload[counter] = 1;
  tx_trigger = 1;
}

counter++;

if(GuestGameDown == HIGH){
  Serial.println('GuestGameDown pressed');
  payload[counter] = 1;
  tx_trigger = 1;
}

counter++;

if(ResetButton == HIGH){
  Serial.print('ResetButton pressed');
  payload[counter] = 1;
  tx_trigger = 1;
}

// add an extra 1 to make sure zero isn't sent as the payload
counter++;
payload[counter] = 1;

// for debugging only
int rand_num = random(1000);
if (rand_num == 12) {
  tx_trigger = 1;
  payload[0] = 1;
}

if (tx_trigger == 1) {  
  write_reg(160, payload_width); // WR_TX_PLOAD: write payload to TX buffer
  digitalWrite(CE, 1);
  delay(1);
  digitalWrite(CE, 0);

  delay(500);

  flush_buf();
}
  
write_reg(39,255); // clear value of STATUS reg
}
