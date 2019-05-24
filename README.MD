# 10ch

Simple project for a 10ch lighting desk
----
## Operation Description

###### Mode 1
Just as a fader/dimmer. IE the fader position on CH1 is translated to output 1, etc.  Master fader controls the overall brightness. The buttons does not do anything unless you press STORE for more than 3 seconds.

If STORE is pressed for more than 3 seconds the system enters into Mode 2 - mode 2 is indicated by CUE LED 1 and 2 flashing 3 times.

###### Mode 2
In mode 2 you can record up to 10 cues, and the fade IN time of the current selected cue. 
How I like it to work:
1 - I set CH faders to levels I wish.
2 - I set the MASTER fader to the time I wish the cue to fade in through. (time need to be indicated somehow)
3 - I use GO and BACK buttons to select what cue I want to store to. Selected cue is indicated by CUE LED flashing. (Cues with “data” in them have lit CUE LEDs, if possible)
4 - I press STORE button to save the current scene + fade time to the selected cue. CUE LED of active cue stops flashing and is on.
5 - Repeat process to store next cue.

Pressing GO and BACK at the same time for more than 3 seconds erases all cues.
If STORE is pressed for more than 3 seconds the system enters into Mode 3 - mode 3 is indicated by CUE LED 1, 2 and 3 flashing 3 times.

Notes:
Initial idea was to have only a fixed fade time, but if it is possible it would be great to have it be selected via the master fader. Hopefully I can manage to fit in a bar graph LED to indicate time. 

###### Mode 3
In mode 3 you can play back the (up to) 10 stored cues in sequence.
How I like it to work:
1 - Starts with no cue active. CUE LED 1 is flashing, indicating that this is the next cue. All other CUE LEDs are off. Output is off.
2 - By pressing GO the outputs fade to the value stored in cue 1 over the time also stored in cue 1. When fade is finished CUE LED 1 is always on, CUE LED 2 is flashing indicating it is the next cue. Output is in cue 1.
3 - By pressing GO the output fade from the current value to the value stored in cue 2 over the time stored in cue 2. When fade is finished CUE LED 1 and 2 are always on, CUE LED 3 is flashing. Output is in cue 2.
4 - Same process as over, just increase numbers by 1.
...
11 - When cue 10 is reached and its fade is finished, system is ready to loop. CUE LED 1 is flashing indicating that cue 1 is next cue, while all other CUE LEDs are on. Output is in cue 10. If GO is pressed, we fade to cue 1, and everything repeats from step 1.

If BACK is pressed we snap?/or fade? to the value of the previous cue. (open to suggestions)
If STORE is pressed for more than 3 seconds the system enters into Mode 1 - mode 1 is indicated by CUE LED 1 flashing 3 times.

Notes:
Would it be possible to have the MASTER fader override the fade time of the cues? So say when it is at 50%, the fade times are like they are stored in the cue. If it is at 0% it is for example 2 times slower. If it is at 100% the fade is 2 times faster? 
This is not important at all, would just be something for the master fader to do in this mode.

## Pinout

| ARDUINO PIN |	ATMEL  | PIN | IS WHAT     | DIRECTION | TYPE           | FUNCTION | NOTES                                                                            |
| ----------- |	------ | --- | ----------- | --------- | ---------------| -------- | -------------------------------------------------------------------------------- |						
| A0          |	PF0    | 97  | Fader CH 8  | IN	       | Analog INPUT	|          | Fader input (also Highlight button for same CH) |
| A1	      | PF1	   | 96  | Fader CH 7  | IN	       | Analog INPUT	|          | Fader input (also Highlight button for same CH) |
| A2          |	PF2	   | 95  | Fader CH 6  | IN	       | Analog INPUT	|          | Fader input (also Highlight button for same CH) |
| A3	      | PF3	   | 94  | Fader CH 5  | IN	       | Analog INPUT	|          | Fader input (also Highlight button for same CH) |
| A4	      | PF4	   | 93  | Fader CH 4  | IN	       | Analog INPUT	|          | Fader input (also Highlight button for same CH) |
| A5	      | PF5	   | 92  | Fader CH 3  | IN	       | Analog INPUT	|          | Fader input (also Highlight button for same CH) |
| A6	      | PF6	   | 91  | Fader CH 2  | IN	       | Analog INPUT	|          | Fader input (also Highlight button for same CH) |
| A7	      | PF7	   | 90  | Fader CH 9  | IN	       | Analog INPUT	|          | Fader input (also Highlight button for same CH) |
| A8	      | PK0	   | 89  | Fader CH 1  | IN	       | Analog INPUT	|          | Fader input (also Highlight button for same CH) |
| A9	      | PK1	   | 88  | Fader CH 10 | IN	       | Analog INPUT	|          | Fader input (also Highlight button for same CH) |
| A10	      | PK2	   | 87  | MASTER      | IN	       | Analog INPUT	|          | Fader input (also Highlight button for same CH) |
|    |    |    |    |    |    |    |    |
| 40	      | PG1	   | 52  | STORE        | IN	       | Digital INPUT	|          | Button, store current state as a cue (Debouncing in HW, but add SW as well) |
| 52	      | PB1	   | 20  | PREVIOUS     | IN	       | Digital INPUT	|          | Button, go back in cue-list (Debouncing in HW, but add SW as well) |
| 53	      | PB0	   | 19  | GO	       | IN	       | Digital INPUT	|          | Button, go forward in cue-ist (Debouncing in HW, but add SW as well) |
|   |    |    |    |    |    |    |    |
| 2	          | PE4	   | 6   | MASTER       | OUT       | PWM OUTPUT		|          | PWM to MASTER indicator LED |
| 3	          | PE5	   | 7   | CH10 	       | OUT       | PWM OUTPUT		|          | PWM to CH10 (also to CH10 indicator LED) |
| 4	          | PG5	   | 1   | CH9	       | OUT       | PWM OUTPUT		|          | PWM to CH9 (also to CH9 indicator LED) |
| 5	          | PE3	   | 5   | CH8    	   | OUT       | PWM OUTPUT		|          | PWM to CH8 (also to CH8 indicator LED) |
| 6	          | PH3	   | 15  | CH7    	   | OUT       | PWM OUTPUT		|          | PWM to CH7 (also to CH7 indicator LED) |
| 7	          | PH4	   | 16  | CH6	       | OUT       | PWM OUTPUT		|          | PWM to CH6 (also to CH6 indicator LED) |
| 8	          | PH5	   | 17  | CH5	       | OUT       | PWM OUTPUT		|          | PWM to CH5 (also to CH5 indicator LED) |
| 9	          | PH6	   | 18  | CH4	       | OUT       | PWM OUTPUT		|          | PWM to CH4 (also to CH4 indicator LED) |
| 10	      | PB4	   | 23  | CH3          | OUT       | PWM OUTPUT		|          | PWM to CH3 (also to CH3 indicator LED) |
| 11	      | PB5	   | 24  | CH2          | OUT       | PWM OUTPUT		|          | PWM to CH2 (also to CH2 indicator LED) |
| 12	      | PB6	   | 25  | CH1	       | OUT       | PWM OUTPUT		|          | PWM to CH1 (also to CH1 indicator LED) |
|    |    |    |    |    |    |    |    |
| 22	      | PA0	   | 78  | Cue 1        | OUT       | Digital OUTPUT	|          | Cue 1 indicator LED |
| 23	      | PA1	   | 77  | Cue 3        | OUT       | Digital OUTPUT	|          | Cue 3 indicator LED |
| 24	      | PA2	   | 76  | Cue 5        | OUT       | Digital OUTPUT	|          | Cue 5 indicator LED |
| 25	      | PA3	   | 75  | Cue 7        | OUT       | Digital OUTPUT	|          | Cue 7 indicator LED |
| 26	      | PA4	   | 74  | Cue 9        | OUT       | Digital OUTPUT	|          | Cue 9 indicator LED |
| 27	      | PA5	   | 73  | Cue 2        | OUT       | Digital OUTPUT	|          | Cue 2 indicator LED |
| 28	      | PA6	   | 72  | Cue 4        | OUT       | Digital OUTPUT	|          | Cue 4 indicator LED |
| 29	      | PA7	   | 71  | Cue 6        | OUT       | Digital OUTPUT	|          | Cue 6 indicator LED |
| 30	      | PC7	   | 60  | Cue 8        | OUT       | Digital OUTPUT	|          | Cue 8 indicator LED |
| 31	      | PC6	   | 59  | Cue 10	   | OUT       | Digital OUTPUT	|          | Cue 10 indicator LED |