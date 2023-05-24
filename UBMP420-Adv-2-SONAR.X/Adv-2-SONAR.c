/*==============================================================================
 Project: Adv-2-SONAR                   Activity: mirobo.tech/ubmp4-adv-2
 Date:    May 24, 2023
 
 This advanced programming activity for the mirobo.tech UBMP4 demonstrates an
 implementation of a simple distance measurement function for readily available
 4-pin HC-SR04 ultrasonic SONAR distance modules. Distance measurement using a
 time delay embedded in a loop is explored as an alternative to a typical
 implementation using a system timer with a follow-on distance calculation.
 
 Attaching a SONAR module to UBMP4
 A 4-pin HC-SR04 sonar module can be connected to two sets of header pins on 
 UBMP4. A set of header pins contains three electrical contacts arranged in a
 commonly used servo configuration: pin 1 (square pad) - an input or output
 signal connected to microcontroller I/O lines, pin 2 (round pad) - +5V supply
 (or Vcc) to power an external device, and pin 3 (round pad) - electrical ground
 (or GND). The four HC-SR04 module connections are often labelled Vcc, TRIG,
 ECHO, and GND.
 
 This example program works using TRIG wired to the H1 signal pin, ECHO wired
 to the H2 signal pin, Vcc connected to the middle +5V supply pin of either H1
 or H2, and GND connected to either of the H1 or H2 ground pins. The program can
 be configured to work on other header pins -- consult the schematic to check
 which header connections are shared by other components of your UBMP4 build. 
==============================================================================*/

#include    "xc.h"              // Microchip XC8 compiler include file
#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include    "UBMP420.h"         // Include UBMP4.2 constants and functions

// TODO Set linker ROM ranges to 'default,-0-7FF' under "Memory model" pull-down.
// TODO Set linker code offset to '800' under "Additional options" pull-down.

// TODO Define SONAR module I/O header pins and modify TRISC for output on TRIG

// SONAR module I/O pin assignment
#define TRIG    LATCbits.LATC0  // SONAR TRIG(ger) output on H1
#define ECHO    PORTCbits.RC1   // SONAR ECHO input on H2

// Program variable definitions
unsigned char distance;         // Target distance in cm
unsigned int timerResult;       // Faux timer result for comparison testing


// SONAR range starter function - return range to the closest target in cm.
unsigned char sonar_range(void)
{
    // Make TRIGger pulse to start a new measurement
    TRIG = 1;
    __delay_us(20);
    TRIG = 0;
    
    // Reset range, wait for ECHO pulse to start
    unsigned char range = 0;
	while(ECHO == 0);           // ECHO=0 during transmit, ECHO=1 during receive
    
    // Count range until ECHO pulse ends
	do {
		__delay_us(58);			// Time delay equivalent to ~2cm of sound travel
		if(range < 255)         // Prevent range from overflowing
        {
            range ++;
        }
	} while(ECHO == 1);         // Repeat until ECHO pulse ends
    
	return(range);              // Return target distance in cm
}

int main(void)
{
    // Set up ports
    OSC_config();               // Configure oscillator for 48 MHz
    UBMP4_config();             // Configure I/O for on-board UBMP4 devices
    
    // Enable output on TRIG pin (match TRISC settings to pin definitions)
    TRISCbits.TRISC0 = 0;       // Set H1 (TRIG) as output pin (H2 remains input)
    
    // Distance measurement pseudo-code
    // timer_config()           // Configure microsecond timer
    // sonar_TRIG()             // Start a new measurement by pulsing TRIG pin
    // timer_clear()            // Reset timer
    // timer_pulse()            // Measure ECHO pulse length (timerResult = us)
    timerResult = 1438;         // Arbitrarily chosen pulse length (microseconds)
    
    // Distance calculation and simulator instruction cycles/run-time stopwatch
    // results for three typically found distance measurement solutions. Set the
    // timerResult variable to a sample pulse length and un-comment one or more
    // of the distance calculations to measure the delay in the simulator.
    
    // distance = (timerResult / 2) * 0.0344;  // 1941 cycles/161.75 microseconds, 48 extra data bytes used
    // distance = timerResult / 29 / 2;    // 527 cycles/43.92 microseconds, 6 extra data bytes used
    // distance = timerResult / 58;        // 475 cycles/39.58 microseconds, 6 extra data bytes used
    
    while(1)
    {
        // Get distance from SONAR module
        distance = sonar_range();   
        
        // Display distance on LEDs
        if(distance > 20)
        {
            LATC = 0b11110000;
        }
        else if(distance > 10)
        {
            LATC = 0b01110000;
        }
        else if(distance > 5)
        {
            LATC = 0b00110000;
        }
        else if (distance > 1)
        {
            LATC = 0b00010000;
        }
        else
        {
            LATC = 0;
        }
        
        __delay_ms(100);        // Do ~10 SONAR pings per second
                
        // Activate bootloader if SW1 is pressed.
        if(SW1 == 0)
        {
            RESET();
        }
    }
}

/* Learn More -- Program Analysis Activities
 * 
 * 1.   The TRISC register controls the PORTC data tristate buffers. What
 *      state must a TRIS bit be in to allow its port pin to be an output?
 * 
 * 2.   This program uses a TRISCbits operator to modify the H1 TRIS bit, but
 *      a logical operation such as the following could have been used instead:

    TRISC = TRISC & 0b11111110; // Set H1 (TRIG) as output, leave H2 as input

 *      Describe an advantage of using a logical operation to selectively clear
 *      TRIS bits over using individual TRISCbits operations.
 * 
 * 3.   What is the largest number that the distance variable can hold? What is
 *      the largest number that the timerResult variable can hold?
 * 
 *      If timerResult is used to measure the SONAR signal's time-of-flight in
 *      microseconds, and distance is meant to represent the calculated distance
 *      to the target object in centimetres, will there be a problem mixing
 *      these two variable types in the program? Explain.
 * 
 * 4.   Program code can be made *better* and more reliable by ensuring that
 *      variable types match each other, or are at least suitable for their
 *      intended purpose and/or large enough for their expected values. Program
 *      code can justifiably also be made *better* by writing it to be smaller,
 *      faster, more memory efficient, or just simpler, and easier to understand
 *      and maintain. All this is to say that there is more than one way to make
 *      program code better, depending on the most valued or most important
 *      features of the application, or characteristics desired by the user or
 *      organization.
 * 
 *      While microcontrollers have gotten faster and more capable over time,
 *      their capabilities are still very limited in comparison with desktop
 *      microprocessors, specifically due to their limited speed and small
 *      memory. Another important factor that needs to be considered in many
 *      microcontroller applications is energy efficiency, usually for the 
 *      purpose of improving battery life. Let's look at some simple ways this
 *      program can be made better by embracing some limitations:
 * 
 *      a) Calculations of the passing of time are literally a waste of time.
 *
 *      A commonly-implemented method of calculating distance from the timed
 *      SONAR ECHO pulse involves a division operation (or a decimal
 *      multiplication, or both). Some microcontrollers, PIC16F1459 included,
 *      do not implement multiplication or division instructions, requiring
 *      software to perform the equivalent operation at a much slower pace. The
 *      48MHz PIC16F1459 used in CHRP4 and UBMP4 is twelve times faster than
 *      the 4MHz PIC16F84, so these math operations are not as relative big a
 *      delay than it would have been in the past, but clock cycles that can be
 *      used for other processing are often wasted on math that does not
 *      actually have to be done! Let's explore why.
 * 
 *      The distance measurement pseudo-code and example calculations in this
 *      program let you demonstrate the math overhead for yourself. The pseudo-
 *      code is representative of a program that would use a microsecond timer
 *      to measure the SONAR pulse length. The first calculation involving two
 *      separate operations is the worst, by taking the most extra time and
 *      using the most extra data memory of all of the examples. Run the code
 *      in the simulator using the stop watch tool to compare your results for
 *      different algorithms and pulse durations.
 *      
 *      A solution to the problem of first measuring microseconds, and then
 *      converting time to its representative distance in centimetres (or any
 *      unit), is to simply measure in multiples of the measurement unit time
 *      duration itself. In this case, a 1cm distance is roughly equivalent to
 *      a round-trip time of 58us (1cm to the target, and 1cm back), so using
 *      58us as the timer value lets the program count centimetres directly
 *      instead of counting microseconds first. To measure in inches, substitute
 *      148us as the timer unit instead. Each timer count can now represent one
 *      distance unit, and the program can simply count loops of the unit-length
 *      delays to determine the distance. When the pulse ends, the loop counter
 *      already contains the distance, and no additional calculations are
 *      required. Not only is the time taken by calculations freed up, this
 *      method also uses less data memory as no additional memory registers are
 *      required during the calculations to maintain precision or match the data
 *      formats of the numbers during the execution of the math algorithms.
 * 
 *      The simplified, no-math distance measurement code can be seen in the
 *      the main loop of the sonar_range() function, where it counts distance-
 *      related units of time instead of microseconds:

    // Count range until ECHO pulse ends
	do {
		__delay_us(58);			// Time delay equivalent to ~2cm of sound travel
		if(range < 255)         // Prevent range from overflowing
        {
            range ++;
        }
	} while(ECHO == 1);         // Repeat until ECHO pulse ends

 *      b) 255cm is enough range (and probably even too much!) for many uses
 * 
 *      One of the applications of CHRP4 circuits with SONAR distance modules
 *      is making simple classroom or maker-space Sumo robots. UBMP4 can be
 *      outfitted with SONAR modules to make a garage parking distance monitor,
 *      or a room alarm people detector. While HC-SR04 modules are advertised
 *      as having 'up to' 400cm (4m) of range, shorter ranges can work just
 *      as well, and may even be preferable to longer detection ranges in some
 *      cases.
 * 
 *      An HC-SR04 distance sensing module in a Sumo robot can have difficulty
 *      reliably detecting another small object (like the opposing robot) at
 *      even 100cm of range. In addition, objects past 100cm would be outside
 *      of the Sumo ring in any case, and should rightly be ignored. So, instead
 *      of creating an integer (int) variable to hold a possible sensor range
 *      of up to 400cm in our program, a byte (char) variable with a maximum
 *      value of 255 will be more than enough for measuring distances up to the
 *      required 80-90cm, and will save one additional byte of data RAM (or
 *      more, if any math is done with the result).
 * 
 *      The range measurement function can be modified to both limit the maximum
 *      range and also to return zero for any measurments beyond the maximum, as
 *      shown by the revised range measurement loop, below:

    // Count range until ECHO pulse ends, or exit early if beyond maxRange
	do {
		__delay_us(58);			// Time delay equivalent to ~2cm of sound travel
		range ++;               // (1cm round trip time to and from target)
		if(range == maxRange)	// Ignore targets beyond maxRange
        {
			return(0);
        }
	} while(ECHO == 1);         // Repeat until ECHO pulse ends or range > max
	return(range);              // Return target distance in cm
 * 
 *      Where does the maxRange value originate from? It could be hard-coded
 *      into the program as a constant, or it could be passes as a variable 
 *      to the sonar_range() function to allow the program to selectively
 *      adjust its sensing distance, if necessary.  You will need to choose one
 *      of these methods to pre-set maxRange before trying the code.

 *      c) Uh-oh, what's changed? The unexpected consequences of simple changes.
 * 
 *      Limiting the maximum range of the distance measurement function has the
 *      added advantage of exiting the range counting do-while loop as soon as
 *      the maximum range threshold is reached. This allows the microcontroller
 *      to continue with other processing rather than waiting (pointlessly) for
 *      the now beyond-limit ECHO pulse to end. But, this creates a new problem.
 *      Can you figure out what it is? Have you run into it?
 * 
 *      Here's a hint: The microcontroller may obtain erratic range readings.
 * 
 *      Ready for the explanation?
 * 
 *      Exiting during a still-in-progress ECHO pulse allows the microcontroller
 *      to continue with other processing and, in simple programs, any other
 *      processing will not take very much time to execute. After completing the
 *      other processing, the microcontroller can start a new TRIG pulse,
 *      without knowing if the previous ECHO is still in progress -- because we
 *      chose to exit the range function before the ECHO pulse ended. To ensure
 *      that our code will not end up re-entering the range-counting loop in
 *      the middle of an ECHO pulse, it is important to check the state of the
 *      ECHO line before triggering a new measurement.
 * 
 *      To clarify, our code either has to wait for an ECHO pulse to complete
 *      *during* the current measurement (as was done in the original program
 *      code), or it has to wait for the prior ECHO pulse to complete *before*
 *      the new measurement, as shown by the code below (which can be added
 *      before the part of the function that creates the trigger pulse):

    // Wait for previous ECHO pulse to finish
    while(ECHO == 1);           // ECHO == 0 when pulse 
    __delay_ms(1);              // Add a slight delay before re-triggering SONAR
 
    // Make TRIGger pulse to start a new measurement
    TRIG = 1;
      .
      .

 *      So, if the SONAR module cannot be re-triggered until the ECHO pulse
 *      ends, was this entire idea of exiting the pulse measurement early 
 *      totally pointless? Not really, as longer distance range measurements
 *      really may not matter for many applications, and exiting the measurement
 *      function early gives the program extra processing time that can be spent
 *      on other tasks. All that is required to take advantage of the extra time
 *      is to not try to re-trigger the module on a fixed schedule, but instead
 *      re-trigger the module when it is ready, as is done here with the
 *      addition of a new, small function to sense the ECHO state:
 
// SONAR ready function - check and return SONAR state (ready == 1). Since
// sonar_range() can exit while the SONAR module is still waiting to receive an
// ECHO, the SONAR module cannot be re-triggered until ECHO goes low.
bool sonar_ready(void)
{
    if(ECHO == 1)
    {
        return(false);          // Not ready - SONAR receive in progress
    }
    return(true);               // SONAR ready
}

 *      With this simple new function, the main code can do a quick call to
 *      determine if the SONAR module is ready, and get a new measurement if it
 *      is, or continue with other work if it's not. Let's put all of these
 *      concepts together in a new range function that allows the programmer
 *      to set a limit on the distance range of interest. The function could be
 *      called with code something like this:

        if(sonar_ready())
        {
            distance = sonar_range(90); // Get target distance if less than 90cm
        }

 *      And, here is the complete, newly revised sonar_range function (you can
 *      use it to replace the original, if you wish):

// SONAR range function - return range to the closest target in cm. Set
// maxRange to ignore objects beyond that distance from the SONAR module.
unsigned char sonar_range(unsigned char maxRange)
{
    // Wait for previous ECHO pulse to finish
    while(ECHO == 1);           // ECHO=0 when pulse ends
    __delay_ms(1);              // Add a delay before re-triggering SONAR
    
    // Make TRIGger pulse to start a new measurement
    TRIG = 1;
    __delay_us(20);
    TRIG = 0;
 
    // Reset range, wait for ECHO pulse to start
    unsigned char range = 0;
	while(ECHO == 0);           // ECHO=0 during transmit, ECHO=1 during receive
    
    // Count range until ECHO pulse ends, or exit early if beyond maxRange
	do {
		__delay_us(58);			// Time delay equivalent to ~2cm of sound travel
		range ++;               // (1cm round trip time to and from target)
		if(range == maxRange)	// Ignore targets beyond maxRange
        {
			return(0);
        }
	} while(ECHO == 1);         // Repeat until ECHO pulse ends or range > max
	return(range);              // Return target distance in cm
}

 * 5.   Both the original and revised SONAR range functions in these examples
 *      make use of a do-while loop structure instead of a typical while loop.
 *      What is the big difference between a while loop and a do-while loop?
 *      Why do you think a do-while loop was used here?     
 */
