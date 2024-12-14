//init some variables, set the freq based on which clock we end up using (based on the chipset)
//
#define framewidth 24 // pulses per data bit

unsigned int outputcode[60];
byte codeLen = 0;
byte bit1;
byte bit2;
byte bit3;
byte bit4;
byte bit5;
byte bit6;

int output_number;
int code_length;
String output_string = "";
String Counted_array = "";

// CRC checksum generation
uint16_t crc_xmodem_update (uint16_t crc, uint8_t data){
  int i; 
  crc = crc ^ ((uint16_t)data << 8);
  for (i=0; i<8; i++){
    if (crc & 0x8000){
      crc = (crc << 1) ^ 0x1021; //(polynomial = 0x1021)
    }
    else{
      crc <<= 1;
    }
  }
  return crc; 
}

uint16_t calc_crc(byte *msg,int n){
  uint16_t x = 0;
  while(n--){
    x = crc_xmodem_update(x, (uint16_t)*msg++);
  }
  return(x);
}

// Timing code constructor
void makeOutputCode(unsigned long tcode) {
  memset(outputcode,0,sizeof(outputcode));
  codeLen = 0;
  
  // split ID into digits
  byte tdigit7 = tcode % 10;
  byte tdigit6 = (tcode / 10) % 10;
  byte tdigit5 = (tcode / 100) % 10;
  byte tdigit4 = (tcode / 1000) % 10;
  byte tdigit3 = (tcode / 10000) % 10;
  byte tdigit2 = (tcode / 100000) % 10;
  byte tdigit1 = (tcode / 1000000) % 10;

  // fill in code values
  byte fullcode[6];
  fullcode[0] = 0xf0 + tdigit1;
  fullcode[1] = 0x00; //00
  fullcode[2] = (tdigit2 << 4) + tdigit3; // 16 x tdigit2 + tdigit3  
  fullcode[3] = (tdigit4 << 4) + tdigit5; // 16 x tdigit4 + tdigit5
  fullcode[4] = (tdigit6 << 4) + tdigit7; // 16 x tdigit6 + tdigit7
  fullcode[5] = 0x00; //00

  // generate CRC check bits
  byte CRC1 [4] = { fullcode[4], fullcode[3], fullcode[2], fullcode[0] }; 
  uint16_t crc_out;
  crc_out = calc_crc(CRC1,4); //Calculate CRC on-the-fly   
 
  // fill in check bits
  fullcode[1] = (crc_out>>8) & 0xff;
  fullcode[5] = crc_out & 0xff;

  // invert all
  fullcode[0] = ~fullcode[0];
  fullcode[1] = ~fullcode[1];
  fullcode[2] = ~fullcode[2];
  fullcode[3] = ~fullcode[3];
  fullcode[4] = ~fullcode[4];
  fullcode[5] = ~fullcode[5];

  // make reversed UART string
  uint32_t UARThigh = 0;
  uint32_t UARTlow = 0;
  
  for (int b = 2; b >= 0; b--) {
    UARTlow = UARTlow << 1;
    UARTlow = UARTlow + 1; // add a 1
    UARTlow = UARTlow << 8;
    UARTlow = UARTlow + fullcode[b];
    UARTlow = UARTlow << 1; // add a zero
  }
  for (int b = 5; b >= 3; b--) {
    UARThigh = UARThigh << 1;
    UARThigh = UARThigh + 1; // add a 1
    UARThigh = UARThigh << 8;
    UARThigh = UARThigh + fullcode[b];
    UARThigh = UARThigh << 1; // add a zero
  }
  
  // count bits to generate timing code
  byte numbits = 0;
  byte lastbit = 0;
  while (UARTlow) {
    if ((UARTlow & B1) != lastbit) {
      outputcode[codeLen] = numbits;
      codeLen++;
      numbits = 0;
      if (lastbit) {
        lastbit = 0;
      } else {
        lastbit = 1;
      }
    } else {
      numbits++;
      UARTlow = UARTlow >> 1;
    }    
  }
  while (UARThigh) {
    if ((UARThigh & B1) != lastbit) {
      outputcode[codeLen] = numbits;
      codeLen++;
      numbits = 0;
      if (lastbit) {
        lastbit = 0;
      } else {
        lastbit = 1;
      }
    } else {
      numbits++;
      UARThigh = UARThigh >> 1;
    }    
  }
  for (int i = 0; i < codeLen; i++) {
    outputcode[i] = outputcode[i] * framewidth;
  }
  output_number = int(outputcode);
  code_length = int(codeLen);
}
