# Rtttl Lib for playing songs on arduino
This is an adaptation of a sample scketch from tone library By Brett Hagman,
it uses Rtttl which is a compact format por mono ringtones.

## Why Rtttl?
It's compact, it's just a string you can paste in your code, no array
construction with frequencies and durations. You can change the octave and tempo
fast and easy.

## What change from the original scketch?
This library was created taking most of the code from an example from Tone library from 'bhagman'.
Code from here: https://github.com/bhagman/Tone/blob/master/examples/RTTTL/RTTTL.pde
The main difference is that the sketch has been converted into a library with new internal 
functions inside the class

To mix the output of the signals to output to a small speaker (i.e. 8 Ohms or higher),
simply use 1K Ohm resistors from each output pin and tie them together at the speaker.
Don't forget to connect the other side of the speaker to ground!

Music is played on a digital pin using the following Advanced I/O functions: 
* tone():   https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/
* noTone(): https://www.arduino.cc/reference/en/language/functions/advanced-io/notone/

There are several RTTTL online players that you can use to test your music
This is one: https://adamonsoon.github.io/rtttl-play/

## More info
RTTTL
- https://en.wikipedia.org/wiki/Ring_Tone_Transfer_Language

You can get more RTTTL (RingTone Text Transfer Language) songs from
- http://code.google.com/p/rogue-code/wiki/ToneLibraryDocumentation



