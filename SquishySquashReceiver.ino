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
  
byte homeSegmentClock = 6;
byte homeSegmentLatch = 5;
byte homeSegmentData = 7;
  
byte awaySegmentClock = 9;
byte awaySegmentLatch = 8;
byte awaySegmentData = 10;
  
byte homeScoreSegmentClock = 3;
byte homeScoreSegmentLatch = 2;
byte homeScoreSegmentData = 4;
  
byte awayScoreSegmentClock = 12;
byte awayScoreSegmentLatch = 13;
byte awayScoreSegmentData = 11;
  
//setting variables for i/o button values
int homeScore = 0;
int homeGame = 0;
int awayScore = 0;
int awayGame = 0;

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
  
  //  flush_buf();
  
  digitalWrite(CE, 1);        // Set CE pin high to enable RX device
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
  
  //  flush_buf();
  
  digitalWrite(CE, 1);        // Set CE pin high to enable RX device
}

void rf_init(void) {
  write_reg(32, 15);    // CONFIG: 2 byte CRC, power up, receiver
  write_reg(33, 63);    // EN_AA: Default, Auto ack all pipe's
  write_reg(34, 3);     // EN_RXADDR: Default, pipe 0 & 1 enabled
  write_reg(35, 3);     // SETUP_AW: Default, 5 byte address
  write_reg(39, 112);     // STATUS: clear all IRQ flags
  
  flush_buf();
}

void setup() {  
  Serial.begin(9600);
  Serial.println("Large Digit Driver Example");

  pinMode(homeSegmentClock, OUTPUT);
  pinMode(homeSegmentData, OUTPUT);
  pinMode(homeSegmentLatch, OUTPUT);

  pinMode(awaySegmentClock, OUTPUT);
  pinMode(awaySegmentData, OUTPUT);
  pinMode(awaySegmentLatch, OUTPUT);

  pinMode(homeScoreSegmentClock, OUTPUT);
  pinMode(homeScoreSegmentLatch, OUTPUT);
  pinMode(homeScoreSegmentData, OUTPUT);

  pinMode(awayScoreSegmentClock, OUTPUT);
  pinMode(awayScoreSegmentLatch, OUTPUT);
  pinMode(awayScoreSegmentData, OUTPUT);

  digitalWrite(homeSegmentClock, LOW);
  digitalWrite(homeSegmentData, LOW);
  digitalWrite(homeSegmentLatch, LOW);

  digitalWrite(awaySegmentClock, LOW);
  digitalWrite(awaySegmentData, LOW);
  digitalWrite(awaySegmentLatch, LOW);

  digitalWrite(homeScoreSegmentClock, LOW);
  digitalWrite(homeScoreSegmentData, LOW);
  digitalWrite(homeScoreSegmentLatch, LOW);

  digitalWrite(awayScoreSegmentClock, LOW);
  digitalWrite(awayScoreSegmentData, LOW);
  digitalWrite(awayScoreSegmentLatch, LOW);

pinMode(MISO, INPUT);  // MISO
pinMode(MOSI, OUTPUT); // MOSI
pinMode(SCK, OUTPUT); // SCK
pinMode(CSN, OUTPUT); // CSN
pinMode(CE, OUTPUT); // CE

digitalWrite(CSN, 1);
digitalWrite(SCK, 0);
digitalWrite(CE, 0);

rf_init();
   
delay(300);
digitalWrite(CSN, 1);
digitalWrite(CE, 0);
delay(100);

rx_mode();
}

void loop() {

showHomeGame(homeGame);
showAwayGame(awayGame);
showHomeScore(homeScore);
showAwayScore(awayScore);

write_reg(7,0); // read value of STATUS reg

byte counter = 0;

Serial.println(spi_status);

if (!(spi_status & 14)) { // RX_DR: RX data received
  write_reg(97, payload_width);  // RD_RX_PLOAD, read received payload

  if (payload[payload_width - 1] == 1) {
    for(counter = 0; counter < payload_width; counter++) {
      Serial.print(" ");
      Serial.print(payload[counter], HEX); // print rx_buf
    }
    Serial.println(" ");
  
    if (payload[0] == 1) homeGame++;
    if (payload[1] == 1) homeGame--;
    if (payload[2] == 1) homeScore++;
    if (payload[3] == 1) homeScore--;
    if (payload[4] == 1) awayGame++;
    if (payload[5] == 1) awayGame--;
    if (payload[6] == 1) awayScore++;
    if (payload[7] == 1) awayScore--;
    if (payload[8] == 1) {
      homeGame = 0;
      homeScore = 0;
      awayGame = 0;
      awayScore = 0;
      Serial.println("reset button pressed...");
    }
  
    Serial.print("home score = ");
    Serial.println(homeScore);
    Serial.print("home games = ");
    Serial.println(homeGame);
    Serial.print("guest score = ");
    Serial.println(awayScore);
    Serial.print("guest games = ");
    Serial.println(awayGame);
    }
  }
    
write_reg(39,255); // clear value of STATUS reg 

delay(100);

}

//Takes a number and displays 2 numbers. Displays absolute value (no negatives)
void showHomeGame(int value) {
  int homeGameNumber = abs(value); //Remove negative signs and any decimals

  //Serial.print("number: ");
  //Serial.println(number);

  for (byte x = 0 ; x < 2 ; x++)
  {
    int remainder = homeGameNumber % 10;

    postHomeGame(remainder, false);

    homeGameNumber /= 10;
  }

  //Latch the current segment data
  digitalWrite(homeSegmentLatch, LOW);
  digitalWrite(homeSegmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}

//Takes a number and displays 2 numbers. Displays absolute value (no negatives)

void showAwayGame(int value) {
  int awayGameNumber = abs(value); //Remove negative signs and any decimals

  //Serial.print("number: ");
  //Serial.println(number);

  for (byte x = 0 ; x < 2 ; x++)
  {
    int remainder = awayGameNumber % 10;

    postAwayGame(remainder, false);

    awayGameNumber /= 10;
  }

  //Latch the current segment data
  digitalWrite(awaySegmentLatch, LOW);
  digitalWrite(awaySegmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}


//Takes a number and displays 2 numbers. Displays absolute value (no negatives)
void showHomeScore(int value) {
  int homeScoreNumber = abs(value); //Remove negative signs and any decimals

  //Serial.print("number: ");
  //Serial.println(number);
  postHomeScore(homeScoreNumber, false);


  //Latch the current segment data
  digitalWrite(homeScoreSegmentLatch, LOW);
  digitalWrite(homeScoreSegmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}

//Takes a number and displays 2 numbers. Displays absolute value (no negatives)
void showAwayScore(int value) {
  int awayScoreNumber = abs(value); //Remove negative signs and any decimals

  //Serial.print("number: ");
  //Serial.println(number);
  postAwayScore(awayScoreNumber, false);


  //Latch the current segment data
  digitalWrite(awayScoreSegmentLatch, LOW);
  digitalWrite(awayScoreSegmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}



//Given a number, or '-', shifts it out to the display
void postHomeGame(int homeNumber, boolean homeDecimal) {
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

  #define a  1<<0
  #define b  1<<6
  #define c  1<<5
  #define d  1<<4
  #define e  1<<3
  #define f  1<<1
  #define g  1<<2
  #define dp 1<<7

  byte segments;

  switch (homeNumber)
  {
    case 1: segments = b | c; break;
    case 2: segments = a | b | d | e | g; break;
    case 3: segments = a | b | c | d | g; break;
    case 4: segments = f | g | b | c; break;
    case 5: segments = a | f | g | c | d; break;
    case 6: segments = a | f | g | e | c | d; break;
    case 7: segments = a | b | c; break;
    case 8: segments = a | b | c | d | e | f | g; break;
    case 9: segments = a | b | c | d | f | g; break;
    case 0: segments = a | b | c | d | e | f; break;
    case ' ': segments = 0; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
  }


  if (homeDecimal) segments |= dp;

  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(homeSegmentClock, LOW);
    digitalWrite(homeSegmentData, segments & 1 << (7 - x));
    digitalWrite(homeSegmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
  //Given a number, or '-', shifts it out to the display
}


void postAwayGame(byte awayNumber, boolean awayDecimal) {
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

  #define a  1<<0
  #define b  1<<6
  #define c  1<<5
  #define d  1<<4
  #define e  1<<3
  #define f  1<<1
  #define g  1<<2
  #define dp 1<<7

  byte segments;

  switch (awayNumber)
  {
    case 1: segments = b | c; break;
    case 2: segments = a | b | d | e | g; break;
    case 3: segments = a | b | c | d | g; break;
    case 4: segments = f | g | b | c; break;
    case 5: segments = a | f | g | c | d; break;
    case 6: segments = a | f | g | e | c | d; break;
    case 7: segments = a | b | c; break;
    case 8: segments = a | b | c | d | e | f | g; break;
    case 9: segments = a | b | c | d | f | g; break;
    case 0: segments = a | b | c | d | e | f; break;
    case ' ': segments = 0; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
  }


  if (awayDecimal) segments |= dp;

  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(awaySegmentClock, LOW);
    digitalWrite(awaySegmentData, segments & 1 << (7 - x));
    digitalWrite(awaySegmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
}

//!!!!!Given a number, or '-', shifts it out to the display
void postHomeScore(byte homeScoreNumber, boolean homeScoreDecimal) {
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

  #define a  1<<0
  #define b  1<<6
  #define c  1<<5
  #define d  1<<4
  #define e  1<<3
  #define f  1<<1
  #define g  1<<2
  #define dp 1<<7

  byte segments;

  switch (homeScoreNumber) {
    case 1: segments = b | c; break;
    case 2: segments = a | b | d | e | g; break;
    case 3: segments = a | b | c | d | g; break;
    case 4: segments = f | g | b | c; break;
    case 5: segments = a | f | g | c | d; break;
    case 6: segments = a | f | g | e | c | d; break;
    case 7: segments = a | b | c; break;
    case 8: segments = a | b | c | d | e | f | g; break;
    case 9: segments = a | b | c | d | f | g; break;
    case 0: segments = a | b | c | d | e | f; break;
    case ' ': segments = 0; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
  }


  if (homeScoreDecimal) segments |= dp;

  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++) {
    digitalWrite(homeScoreSegmentClock, LOW);
    digitalWrite(homeScoreSegmentData, segments & 1 << (7 - x));
    digitalWrite(homeScoreSegmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
}

//Given a number, or '-', shifts it out to the display
void postAwayScore(byte awayScoreNumber, boolean awayScoreDecimal) {
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

  #define a  1<<0
  #define b  1<<6
  #define c  1<<5
  #define d  1<<4
  #define e  1<<3
  #define f  1<<1
  #define g  1<<2
  #define dp 1<<7

  byte segments;

  switch (awayScoreNumber) {
    case 1: segments = b | c; break;
    case 2: segments = a | b | d | e | g; break;
    case 3: segments = a | b | c | d | g; break;
    case 4: segments = f | g | b | c; break;
    case 5: segments = a | f | g | c | d; break;
    case 6: segments = a | f | g | e | c | d; break;
    case 7: segments = a | b | c; break;
    case 8: segments = a | b | c | d | e | f | g; break;
    case 9: segments = a | b | c | d | f | g; break;
    case 0: segments = a | b | c | d | e | f; break;
    case ' ': segments = 0; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
  }


  if (awayScoreDecimal) segments |= dp;

  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++) {
    digitalWrite(awayScoreSegmentClock, LOW);
    digitalWrite(awayScoreSegmentData, segments & 1 << (7 - x));
    digitalWrite(awayScoreSegmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
}
