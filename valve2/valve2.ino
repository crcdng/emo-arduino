#define MINSEQ 0
#define MAXSEQ (sizeof(sequences)/sizeof(sequences[0]))
#define ENDSEQ (-1)

// these are the lists of the valves to open in each sequence. So sequence 1 opens valves 1-4
// for 3 consecutive 400ms periodsfollowed by no valves for 4 periods.
// sequnce 4 does the same but using only valve 5, etc.
// the sequences repeat when they reach the end.

int seq1[] = { 0b001111, 0b001111, 0b001111, 0b000000, 0b000000, 0b000000, 0b000000, ENDSEQ };
int seq2[] = { 0b001111, 0b000000, 0b001111, 0b000000, ENDSEQ };
int seq3[] = { 0b001001, 0b000011, 0b000111, 0b000101, 0b000000, 0b000110, 0b001110, ENDSEQ };
int seq4[] = { 0b010000, 0b010000, 0b010000, 0b000000, 0b000000, 0b000000, 0b000000, ENDSEQ };
int seq5[] = { 0b000000, 0b000000, 0b000000, 0b000000, 0b010000, 0b010000, 0b000000, 0, 0, 0, ENDSEQ };

// the button steps through the choice of sequences in this list

int *sequences[] = { seq1, seq2, seq3, seq4, seq5 };


void setup() {
  // 
  Serial.begin(115200);

  // set output drivers and turn all off
  pinMode(2,OUTPUT); digitalWrite(2,0);
  pinMode(3,OUTPUT); digitalWrite(3,0);
  pinMode(4,OUTPUT); digitalWrite(4,0);
  pinMode(5,OUTPUT); digitalWrite(5,0);
  pinMode(6,OUTPUT); digitalWrite(6,0);
  pinMode(7,OUTPUT); digitalWrite(7,0);

  pinMode(10,INPUT_PULLUP);
  pinMode(12,OUTPUT);digitalWrite(12,0);
  pinMode(11,OUTPUT);digitalWrite(11,1);

  // led off
  pinMode(13,OUTPUT);digitalWrite(13, 0);
}

void loop() {
  // show switch status with led
  digitalWrite(13, !digitalRead(10));

  // flip between sequences when the button / reed switch is operated

  static int debounce = 0;
  debounce += digitalRead(10) ? -1 : 1;
  if (debounce > 10) debounce = 10;
  if (debounce < -10) debounce = -10;

  static int seq = MINSEQ, lastseq = MINSEQ;
  static int index = 0;
  
  if (debounce > 5) {
    lastseq = seq;
  }

  if ((debounce == -10) && (seq == lastseq)) {
    seq = lastseq + 1;
    if (seq >= MAXSEQ)
      seq = MINSEQ;
    index = 0;
  }

  // step though the elements in the currently selected sequence. 
  // the step time is fixed at 400ms.

#define TIMER 400

  static long timer = TIMER;
   
  if (--timer <= 0) {
    timer = TIMER;
    ++index;
    if (sequences[seq][index] == ENDSEQ)
      index = 0;

    if (1)
    {   
      Serial.print("debounce ");
      Serial.print(debounce);

      Serial.print(" sequence ");
      Serial.print(seq);

      Serial.print(" index ");
      Serial.print(index);

      Serial.print(" mask ");
      Serial.print((sequences[seq])[index], HEX);

      Serial.print(" last seq ");
      Serial.print(lastseq);

      Serial.println();
    }  
  }

  // output current state. This is locked around ports 2-8 .. should have a map for those really.
  int port, mask;
  for (port = 2, mask = 1; port < 8; ++port, mask <<= 1) {
    digitalWrite(port, ((sequences[seq])[index] & mask) ? 1  :0);
  }
  
  // establish loop time to control debounce and sequence timing
  delay(10);


}

