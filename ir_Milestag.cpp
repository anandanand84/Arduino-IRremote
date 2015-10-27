/*
Assuming the protocol we are adding is for the (imaginary) manufacturer:  Milestag

Our fantasy protocol is a standard protocol, so we can use this standard
template without too much work. Some protocols are quite unique and will require
considerably more work in this file! It is way beyond the scope of this text to
explain how to reverse engineer "unusual" IR protocols. But, unless you own an
oscilloscope, the starting point is probably to use the rawDump.ino sketch and
try to spot the pattern!

Before you start, make sure the IR library is working OK:
  # Open up the Arduino IDE
  # Load up the rawDump.ino example sketch
  # Run it

Now we can start to add our new protocol...

1. Copy this file to : ir_Milestag.cpp

2. Replace all occurrences of "Milestag" with the name of your protocol.

3. Tweak the #defines to suit your protocol.

4. If you're lucky, tweaking the #defines will make the default send() function
   work.

5. Again, if you're lucky, tweaking the #defines will have made the default
   decode() function work.

You have written the code to support your new protocol!

Now you must do a few things to add it to the IRremote system:

1. Open IRremote.h and make the following changes:
   REMEMEBER to change occurences of "MILESTAG" with the name of your protocol

   A. At the top, in the section "Supported Protocols", add:
      #define DECODE_MILESTAG  1
      #define SEND_MILESTAG    1

   B. In the section "enumerated list of all supported formats", add:
      MILESTAG,
      to the end of the list (notice there is a comma after the protocol name)

   C. Further down in "Main class for receiving IR", add:
      //......................................................................
      #if DECODE_MILESTAG
          bool  decodeMilestag (decode_results *results) ;
      #endif

   D. Further down in "Main class for sending IR", add:
      //......................................................................
      #if SEND_MILESTAG
          void  sendMilestag (unsigned long data,  int nbits) ;
      #endif

   E. Save your changes and close the file

2. Now open irRecv.cpp and make the following change:

   A. In the function IRrecv::decode(), add:
      #ifdef DECODE_NEC
          DBG_PRINTLN("Attempting Milestag decode");
          if (decodeMilestag(results))  return true ;
      #endif

   B. Save your changes and close the file

You will probably want to add your new protocol to the example sketch

3. Open MyDocuments\Arduino\libraries\IRremote\examples\IRrecvDumpV2.ino

   A. In the encoding() function, add:
      case MILESTAG:    Serial.print("MILESTAG");     break ;

Now open the Arduino IDE, load up the rawDump.ino sketch, and run it.
Hopefully it will compile and upload.
If it doesn't, you've done something wrong. Check your work.
If you can't get it to work - seek help from somewhere.

If you get this far, I will assume you have successfully added your new protocol
There is one last thing to do.

1. Delete this giant instructional comment.

2. Send a copy of your work to us so we can include it in the library and
   others may benefit from your hard work and maybe even write a song about how
   great you are for helping them! :)

Regards,
  BlueChip
*/

#include "IRremote.h"
#include "IRremoteInt.h"

//==============================================================================
//
//
//                              S H U Z U
//
//
//==============================================================================

#define MIN_BITS   	32  // The number of bits in the command

#define HDR_MARK    2400  // The length of the Header:Mark
#define SPACE   	600  // The lenght of the Space

#define BIT_MARK    1200  // The length of a Bit:Mark
#define ZERO_MARK   600  // The length of a Bit:Space for 1's

//+=============================================================================
//
#if SEND_MILESTAG
void  IRsend::sendMilestag (unsigned long data,  int nbits)
{
	// Set IR carrier frequency
	enableIROut(40);

	// Header
	mark(HDR_MARK);
	space(SPACE);

	// Data
	for (unsigned long  mask = 1UL << (nbits - 1);  mask;  mask >>= 1) {
		if (data & mask) {
			mark(BIT_MARK);
			space(SPACE);
		} else {
			mark(ZERO_MARK);
			space(SPACE);
    	}
  	}
}
#endif

//+=============================================================================
//
#if DECODE_MILESTAG
bool  IRrecv::decodeMilestag (decode_results *results)
{
	long  data   = 0;
	int   offset = 0;  // Dont skip first space, check its size

	//we will decide on the min and max bits later
	//if (irparams.rawlen < (2 * SONY_BITS) + 2)  return false ;

	offset++;

	// Initial mark
	if (!MATCH_MARK(results->rawbuf[offset++], HDR_MARK))  return false ;

	while (offset + 1 < irparams.rawlen) {
		if (!MATCH_SPACE(results->rawbuf[offset++], SPACE))  break ;

		if      (MATCH_MARK(results->rawbuf[offset], BIT_MARK))   data = (data << 1) | 1 ;
		else if (MATCH_MARK(results->rawbuf[offset], ZERO_MARK))  data = (data << 1) | 0 ;
		else                                                           return false ;
		offset++;
	}

	// Success
	results->bits = (offset - 1) / 2;

	results->value       = data;
	results->decode_type = MILESTAG;
	return true;

}
#endif
