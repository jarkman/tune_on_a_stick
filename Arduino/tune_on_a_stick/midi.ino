#include <SoftwareSerial.h>



// define the pins used
#define VS1053_RX  2 // This is the pin that connects to the RX pin on VS1053
#define VS1053_RESET 9 // This is the pin that connects to the RESET pin on VS1053
// Don't forget to connect the GPIO #0 to GROUND and GPIO #1 pin to 3.3V

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 31
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 32 for more!
#define VS1053_GM1_ACOUSTIC_GRAND_PIANO 0x01
#define VS1053_GM1_HONKY_TONK_PIANO 4
#define VS1053_GM1_ELECTRCIC_GUITAR_JAZZ 27
#define VS1053_GM1_MUTED_TRUMPET 60
#define VS1053_GM1_OCARINA 80
#define VS1053_GM1_MELODIC_TOM 118
#define VS1053_GM1_SLAP_BASS_1 37
#define VS1053_GM1_SYNTH_BASS_1 39

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0

// http://www.midi.org/techspecs/midimessages.php#3
#define MIDI_SYSTEM_EXCLUSIVE 0xF0
#define MIDI_REALTIME 0x7F
#define MIDI_ALL_DEVICES 0x7F
#define MIDI_DEVICE_CONTROL 0x04
#define MIDI_MASTER_VOLUME 0x01
#define MIDI_SYSTEM_EXCLUSIVE_END 0xF7



#ifdef SPARKFUN_SHIELD
    // For Sparkfun MIDI shield
    #define VS1053_MIDI Serial
#else
    SoftwareSerial VS1053_MIDI(0, 2); // TX only, do not use the 'rx' side
    // on a Mega/Leonardo you may have to change the pin to one that 
    // software serial support uses OR use a hardware serial port!
#endif

void setup_midi()
{
  
   VS1053_MIDI.begin(31250); // MIDI uses a 'strange baud rate'

#ifndef SPARKFUN_SHIELD
   pinMode(VS1053_RESET, OUTPUT);
   digitalWrite(VS1053_RESET, LOW);
   delay(100);
   digitalWrite(VS1053_RESET, HIGH);
   delay(10);
#endif
  
   // midiSetMasterVolume( 0x0FFF ); // 0x1FFF is half volumne
  
   // Lead instrument
#ifndef SPARKFUN_SHIELD
   midiSetChannelBank(0, VS1053_BANK_MELODY);
#endif

   //midiSetInstrument(0, VS1053_GM1_OCARINA);
   //midiSetInstrument(0, VS1053_GM1_MELODIC_TOM);
   midiSetInstrument(0, VS1053_GM1_ACOUSTIC_GRAND_PIANO);
   midiSetChannelVolume(0, 127);
  
  
   // Backing instrument
#ifndef SPARKFUN_SHIELD
   midiSetChannelBank(1, VS1053_BANK_MELODY);
#endif

   midiSetInstrument(1, VS1053_GM1_SYNTH_BASS_1);
   midiSetChannelVolume(1, MIDI_BACKING_VOLUME);

   // Harmony instrument
#ifndef SPARKFUN_SHIELD
   midiSetChannelBank(2, VS1053_BANK_MELODY);
#endif

   midiSetInstrument(2, VS1053_GM1_SLAP_BASS_1);
   midiSetChannelVolume(2, MIDI_HARMONY_VOLUME);
   
  // 10 is the magic channel for percussion
#ifndef SPARKFUN_SHIELD
   midiSetChannelBank(PERCUSSION_CHANNEL, VS1053_BANK_DRUMS2); 
   midiSetInstrument(PERCUSSION_CHANNEL, 82); // not sure why we have to set an instrument, since the note number sets the instrument. Odd.
#endif
   midiSetChannelVolume(PERCUSSION_CHANNEL, MIDI_PERCUSSION_VOLUME);
  
}

void pickRandomInstruments()
{
     midiSetInstrument(0, random(96));
     midiSetInstrument(1, random(96));
     midiSetInstrument(2, random(96));
}

void midi_note( int line, int note )
{
  if( note > 0 )
      midiNoteOn(line, note, 127);
}

void midi_note_off( int line, int note )
{
  if( note > 0 )
      midiNoteOff(line, note, 127);
}

void midi_boom( )
{
      midiNoteOn(PERCUSSION_CHANNEL, 35, 127);
}

void midi_tish( )
{
      midiNoteOn(PERCUSSION_CHANNEL, 42, 127);
}

void midiSetInstrument(uint8_t chan, uint8_t inst) {
  if (chan > 15) return;
  inst --; // page 32 has instruments starting with 1 not 0 :(
  if (inst > 127) return;
  
  VS1053_MIDI.write(MIDI_CHAN_PROGRAM | chan);  
  VS1053_MIDI.write(inst);
}


void midiSetMasterVolume( uint8_t vol) {
  // RPS - data sheet claims this is supported, but it doesn't seem to work for me
  // http://www.recordingblogs.com/sa/tabid/88/Default.aspx?topic=MIDI+Master+Volume+message
  if (vol > 0x3FFF) return;
  
  int low_7_bits = vol & 0x007F;
  int high_7_bits = vol >> 7;
  
  VS1053_MIDI.write( MIDI_SYSTEM_EXCLUSIVE);
  VS1053_MIDI.write( MIDI_REALTIME);
  VS1053_MIDI.write( MIDI_ALL_DEVICES);
  VS1053_MIDI.write( MIDI_DEVICE_CONTROL );
  VS1053_MIDI.write( MIDI_MASTER_VOLUME );
  VS1053_MIDI.write(low_7_bits);
  VS1053_MIDI.write(high_7_bits);
    VS1053_MIDI.write( MIDI_SYSTEM_EXCLUSIVE_END);

}

void midiSetChannelVolume(uint8_t chan, uint8_t vol) {
  if (chan > 15) return;
  if (vol > 127) return;
  
  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(MIDI_CHAN_VOLUME);
  VS1053_MIDI.write(vol);
}

void midiSetChannelBank(uint8_t chan, uint8_t bank) {
  if (chan > 15) return;
  if (bank > 127) return;
  
  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write((uint8_t)MIDI_CHAN_BANK);
  VS1053_MIDI.write(bank);
}

void midiNoteOn(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;
  
  VS1053_MIDI.write(MIDI_NOTE_ON  | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void midiNoteOff(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;
  
  VS1053_MIDI.write(MIDI_NOTE_OFF | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void midiAllNotesOff(uint8_t chan ) {
  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(0x7b);
  VS1053_MIDI.write((uint8_t) 0x00);
}

