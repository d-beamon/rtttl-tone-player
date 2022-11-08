// This library was created taking most of the code from an example from Tone library from 'bhagman'.
// Code from here: https://github.com/bhagman/Tone/blob/master/examples/RTTTL/RTTTL.pde

// To mix the output of the signals to output to a small speaker (i.e. 8 Ohms or higher),
// simply use 1K Ohm resistors from each output pin and tie them together at the speaker.
// Don't forget to connect the other side of the speaker to ground!

// You can get more RTTTL (RingTone Text Transfer Language) songs from
// http://code.google.com/p/rogue-code/wiki/ToneLibraryDocumentation

// Music is played on a digital pin using the following Advanced I/O functions: 
// * tone():   https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/
// * noTone(): https://www.arduino.cc/reference/en/language/functions/advanced-io/notone/

// There are several RTTTL online players that you can use to test your music
// This is one: https://adamonsoon.github.io/rtttl-play/

/******************************************************************************
 * INCLUDE
 ******************************************************************************/

#include "RTTTL_player.h"
#include "pitches.h"

/******************************************************************************
 * MACROS
 ******************************************************************************/
 
#define OCTAVE_OFFSET 0

#define RTTTL_LOG(lvl,args...)    { if ((lvl) <= _loglevel) { Serial.print(args); } }
#define RTTTL_LOGln(lvl,args...)  { if ((lvl) <= _loglevel) { Serial.println(args); } }

#define isdigit(n) (n >= '0' && n <= '9')

/******************************************************************************
 * LOCAL VARIABLES
 ******************************************************************************/

static int notes[] = { 0,
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
  NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
  NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
  NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
};

/******************************************************************************
   CTOR/DTOR
 ******************************************************************************/
 
/***********************************************/
/**
 ***********************************************/
RTTTL_player::RTTTL_player()
{
}

/******************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 ******************************************************************************/

/**
 * Change the log level
 * @param new_loglevel New log level
 */
void RTTTL_player::loglevel_set(int new_loglevel)
{
  _loglevel = new_loglevel;
}


/**
 * Play a song on a PIN
 * @param pin PIN number where the song will be played
 * @param song Pointer to the RTTTL song
 */
void RTTTL_player::play(int pin, char *song)
{
  char *p = song;
  char *song_name = NULL;

  byte default_dur = 4;
  byte default_oct = 6;
  int bpm = 63;
  
  int num;
  long wholenote;
  long duration;
  byte note;
  byte scale;

  if (!song) {
    RTTTL_LOGln(1, "rtttl_play: ERROR: No song defined");
    return;
  }

  RTTTL_LOG(3, "rtttl_play: "); RTTTL_LOGln(3, song);

  // find the start (skip name, etc)
  song_name = get_song_name(song, &p);
  RTTTL_LOG(3, "Song name: "); RTTTL_LOGln(3, song_name);
  
  // format: d=N,o=N,b=NNN:
  get_header(song, &p, &default_dur, &default_oct, &bpm);

  // BPM usually expresses the number of quarter notes per minute
  wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)

  RTTTL_LOG(5, "wn: "); RTTTL_LOGln(5, wholenote, 10);

  // now begin note loop
  while(*p)
  {
    get_note(song, &p, &duration, &note, &scale,
          wholenote, default_dur, default_oct);

    // now play the note
    if(note)
    {
      RTTTL_LOG(5, "Playing: ");
      RTTTL_LOG(5, scale, 10); RTTTL_LOG(5, ' ');
      RTTTL_LOG(5, note, 10); RTTTL_LOG(5, " (");
      RTTTL_LOG(5, notes[(scale - 4) * 12 + note], 10);
      RTTTL_LOG(5, ") ");
      RTTTL_LOGln(5, duration, 10);
      tone(pin, notes[(scale - 4) * 12 + note], duration);
    }
    else
    {
      RTTTL_LOG(5, "Pausing: ");
      RTTTL_LOGln(5, duration, 10);
      noTone(pin);
    }
    delay(duration);
  }

  RTTTL_LOGln(3, "rtttl_play: End song!");
}

/******************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 ******************************************************************************/

/**
 * Get the song name
 * 
 * @param song Pointer to the RTTTL song
 * @param ptr Double pointer where the pointer to the next char after the song name will point after the function
 * @return Pointer to the song name (static variable inside the function)
 */
char *RTTTL_player::get_song_name(char *song, char **ptr)
{
  static char songname[32];
  
  // Skip songname
  char *p = song;
  while(*p != ':') p++;    // ignore name
  //*p = 0;                     // skip ':'
  p++;
  *ptr = p;
  
  // Return songname
  int songname_len = min(p - song - 1, 31);
  memcpy(songname, song, songname_len);
  songname[songname_len] = 0;
  
  return songname;
}

/**
 * Get the header of the song
 * format: d=N,o=N,b=NNN:
 * 
 * @param song[in] Pointer to the RTTTL song
 * @param ptr[in/out] Double pointer where the pointer to the next char after the song name will point after the function
 * @param default_dur[out] Pointer where the default duration will be stored
 * @param default_oct[out] Pointer where the default octave will be stored
 * @param bpm[out] Pointer where the BPM will be stored
 * @return 0 if success, -1 if error
 */
int RTTTL_player::get_header(char *song, char **ptr, byte *default_dur, byte *default_oct, int *bpm)
{
  char *p = *ptr;
  int num = 0;
  
  // get default duration
  if(*p == 'd')
  {
    p++; p++;              // skip "d="
    num = 0;
    while(isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    if(num > 0) *default_dur = num;
    p++;                   // skip comma
  }

  RTTTL_LOG(5, "ddur: "); RTTTL_LOGln(5, *default_dur, 10);

  // get default octave
  if(*p == 'o')
  {
    p++; p++;              // skip "o="
    num = *p++ - '0';
    if(num >= 3 && num <=7) *default_oct = num;
    p++;                   // skip comma
  }

  RTTTL_LOG(5,"doct: "); RTTTL_LOGln(5, *default_oct, 10);

  // get BPM
  if(*p == 'b')
  {
    p++; p++;              // skip "b="
    num = 0;
    while(isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    *bpm = num;
    p++;                   // skip colon
  }

  RTTTL_LOG(5, "bpm: "); RTTTL_LOGln(5, *bpm, 10);
  *ptr = p;
  return 0;
}


/**
 * Get the next note
 * 
 * @param song[in] Pointer to the RTTTL song
 * @param ptr[in/out] Double pointer where the pointer to the next char after the song name will point after the function
 * @param duration[out] Pointer where the duration will be stored
 * @param note[out] Pointer where the note will be stored
 * @param scale[out] Pointer where the scale will be stored
 * @param wholenote[in] Duration of a whole note
 * @param default_dur[in] Default duration of a note
 * @param default_oct[in] Default octave of a note
 * @return 0 if success, -1 if error
 */
int RTTTL_player::get_note(char *song, char **ptr, 
      long *duration, byte *note, byte *scale,
      long wholenote, byte default_dur, byte default_oct)
{
  char *p = *ptr;
  // first, get note duration, if available
  int num = 0;
  while(isdigit(*p))
  {
    num = (num * 10) + (*p++ - '0');
  }

  if(num) *duration = wholenote / num;
  else *duration = wholenote / default_dur;  // we will need to check if we are a dotted note after

  // now get the note
  *note = 0;

  switch(*p)
  {
    case 'c':
      *note = 1;
      break;
    case 'd':
      *note = 3;
      break;
    case 'e':
      *note = 5;
      break;
    case 'f':
      *note = 6;
      break;
    case 'g':
      *note = 8;
      break;
    case 'a':
      *note = 10;
      break;
    case 'b':
      *note = 12;
      break;
    case 'p':
    default:
      *note = 0;
  }
  p++;

  // now, get optional '#' sharp
  if(*p == '#')
  {
    //RTTTL_LOGln(5, "Sharp");

    (*note)++;
    p++;
  }

  // now, get optional '.' dotted note
  if(*p == '.')
  {
    *duration += (*duration) / 2;
    p++;
  }

  // now, get scale
  if(isdigit(*p))
  {
    *scale = *p - '0';
    p++;
  }
  else
  {
    *scale = default_oct;
  }

  *scale += OCTAVE_OFFSET;

  if(*p == ',')
    p++;       // skip comma for next note (or we may be at the end)

  *ptr = p;
  return 0;
}
