//******************************************************************************
// IRremote
// Origial code by Ken Shirriff
// Modified by Paul Stoffregen <paul@pjrc.com> to support other boards and timers
// Modified by Michael Rickert <Slacker87> for more accurate IR LED timings
// Interrupt code based on NECIRrcv by Joe Knapp
//******************************************************************************

#define EXTERN extern
#define RAWBUF  101  // Maximum length of raw duration buffer

typedef
  struct {
    // The fields are ordered to reduce memory over caused by struct-padding
    uint8_t       rcvstate;        // State Machine state
    uint8_t       blinkpin;
    uint8_t       blinkflag;       // true -> enable blinking of pin on IR processing
    uint8_t       rawlen;          // counter of entries in rawbuf
    unsigned int  timer;           // State timer, counts 50uS ticks.
    unsigned int  rawbuf[RAWBUF];  // raw data
    uint8_t       overflow;        // Raw buffer overflow occurred
  }
irparams_t;

#define STATE_IDLE      2
#define STATE_MARK      3
#define STATE_SPACE     4
#define STATE_STOP      5
#define STATE_OVERFLOW  6

EXTERN  volatile irparams_t  irparams;

#define BLINKLED        13
#define BLINKLED_ON()  (PORTB |= B00100000)
#define BLINKLED_OFF()  (PORTB &= B11011111)

// microseconds per clock interrupt tick
#define USECPERTICK 50
#define SYSCLOCK  F_CPU     // main Arduino clock

// Defines for setting and clearing register bits
#define cbi(sfr, bit)  (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit)  (_SFR_BYTE(sfr) |= _BV(bit))

#define TIMER_RESET
#define TIMER_ENABLE_PWM     (TCCR3A |= _BV(COM3A1))
#define TIMER_DISABLE_PWM    (TCCR3A &= ~(_BV(COM3A1)))
#define TIMER_INTR_NAME      TIMER3_COMPA_vect

#define TIMER_CONFIG_KHZ(val) ({ \
  const uint16_t pwmval = SYSCLOCK / 2000 / (val); \
  TCCR3A = _BV(WGM31); \
  TCCR3B = _BV(WGM33) | _BV(CS30); \
  ICR3 = pwmval; \
  OCR3A = pwmval / 3; \
})

#define TIMER_PWM_PIN 5
