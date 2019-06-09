# 10ch

Simple project for a 10ch lighting desk
----
### Operation Description

###### Mode select
If STORE is pressed for more than 3 seconds the system enters into Mode select. Pressing go and backwards cycles through modes.

###### Mode 1
Just as a fader/dimmer. IE the fader position on CH1 is translated to output 1, etc.  Master fader controls the overall brightness.

###### Mode 2
In mode 2 you can record up to 10 cues, and the fade time of the current selected cue. 

* 1 - Set CH faders to levels.
* 2 - Set the MASTER fader to fade time (0 - 10s, indicated in timer LEDs).
* 3 - Use GO and BACK buttons to select what cue to store to. Selected cue is indicated by CUE LED: this is flashing if no data is present or constant otherwise.
* 4 - Press STORE button to save the current scene + fade time to the selected cue.

Pressing GO and BACK at the same time for more than 3 seconds erases all cues.
This is indicated with a boot sequence on all LEDs.

###### Mode 3
In mode 3 you can play back the 10 stored cues in sequence.

* 1 - Starts with no cue active. CUE LED 1 is flashing, indicating that this is the next cue.
* 2 - By pressing GO or BACK we cycle forwards and backwards through the recorded cues. The cues indicator trail towards the selected cue

If BACK is pressed we fade to the value of the previous cue using its fade time.

Values from cue are mixed with values from faders (HTP), master acts as normal.

### Pinout

| ARDUINO PIN |	ATMEL  | PIN | IS WHAT     | DIRECTION | TYPE           | NOTES                                                                            |
| ----------- |	------ | --- | ----------- | --------- | ---------------| -------------------------------------------------------------------------------- |						
| A0          |	PF0    | 97  | Fader CH 8  | IN	       | Analog INPUT	| Fader input (also Highlight button for same CH) |
| A1	      | PF1	   | 96  | Fader CH 7  | IN	       | Analog INPUT	| Fader input (also Highlight button for same CH) |
| A2          |	PF2	   | 95  | Fader CH 6  | IN	       | Analog INPUT	| Fader input (also Highlight button for same CH) |
| A3	      | PF3	   | 94  | Fader CH 5  | IN	       | Analog INPUT	| Fader input (also Highlight button for same CH) |
| A4	      | PF4	   | 93  | Fader CH 4  | IN	       | Analog INPUT	| Fader input (also Highlight button for same CH) |
| A5	      | PF5	   | 92  | Fader CH 3  | IN	       | Analog INPUT	| Fader input (also Highlight button for same CH) |
| A6	      | PF6	   | 91  | Fader CH 2  | IN	       | Analog INPUT	| Fader input (also Highlight button for same CH) |
| A7	      | PF7	   | 90  | Fader CH 9  | IN	       | Analog INPUT	| Fader input (also Highlight button for same CH) |
| A8	      | PK0	   | 89  | Fader CH 1  | IN	       | Analog INPUT	| Fader input (also Highlight button for same CH) |
| A9	      | PK1	   | 88  | Fader CH 10 | IN	       | Analog INPUT	| Fader input (also Highlight button for same CH) |
| A10	      | PK2	   | 87  | MASTER      | IN	       | Analog INPUT	| Fader input (also Highlight button for same CH) |
|             |        |     |             |           |                |                                                 |
| 40	      | PG1	   | 52  | STORE       | IN	       | Digital INPUT	| Button, store current state as a cue            |
| 52	      | PB1	   | 20  | PREVIOUS    | IN	       | Digital INPUT	| Button, go back in cue-list                     |
| 53	      | PB0	   | 19  | GO	       | IN	       | Digital INPUT	| Button, go forward in cue-ist                   |
|             |        |     |             |           |                |                                                 |
| 2	          | PE4	   | 6   | MASTER      | OUT       | PWM OUTPUT		| PWM to MASTER indicator LED                     |
| 3	          | PE5	   | 7   | CH10 	   | OUT       | PWM OUTPUT		| PWM to CH10 (also to CH10 indicator LED)        |
| 4	          | PG5	   | 1   | CH9	       | OUT       | PWM OUTPUT		| PWM to CH9 (also to CH9 indicator LED)          |
| 5	          | PE3	   | 5   | CH8    	   | OUT       | PWM OUTPUT		| PWM to CH8 (also to CH8 indicator LED)          |
| 6	          | PH3	   | 15  | CH7    	   | OUT       | PWM OUTPUT		| PWM to CH7 (also to CH7 indicator LED)          |
| 7	          | PH4	   | 16  | CH6	       | OUT       | PWM OUTPUT		| PWM to CH6 (also to CH6 indicator LED)          |
| 8	          | PH5	   | 17  | CH5	       | OUT       | PWM OUTPUT		| PWM to CH5 (also to CH5 indicator LED)          |
| 9	          | PH6	   | 18  | CH4	       | OUT       | PWM OUTPUT		| PWM to CH4 (also to CH4 indicator LED)          |
| 10	      | PB4	   | 23  | CH3         | OUT       | PWM OUTPUT		| PWM to CH3 (also to CH3 indicator LED)          |
| 11	      | PB5	   | 24  | CH2         | OUT       | PWM OUTPUT		| PWM to CH2 (also to CH2 indicator LED)          |
| 12	      | PB6	   | 25  | CH1	       | OUT       | PWM OUTPUT		| PWM to CH1 (also to CH1 indicator LED)          |
|             |        |     |             |           |                |                                                 |
| 22	      | PA0	   | 78  | Cue 1       | OUT       | Digital OUTPUT	| Cue 1 indicator LED                             |
| 23	      | PA1	   | 77  | Cue 3       | OUT       | Digital OUTPUT	| Cue 3 indicator LED                             |
| 24	      | PA2	   | 76  | Cue 5       | OUT       | Digital OUTPUT	| Cue 5 indicator LED                             |
| 25	      | PA3	   | 75  | Cue 7       | OUT       | Digital OUTPUT	| Cue 7 indicator LED                             |
| 26	      | PA4	   | 74  | Cue 9       | OUT       | Digital OUTPUT	| Cue 9 indicator LED                             |
| 27	      | PA5	   | 73  | Cue 2       | OUT       | Digital OUTPUT	| Cue 2 indicator LED                             |
| 28	      | PA6	   | 72  | Cue 4       | OUT       | Digital OUTPUT	| Cue 4 indicator LED                             |
| 29	      | PA7	   | 71  | Cue 6       | OUT       | Digital OUTPUT	| Cue 6 indicator LED                             |
| 30	      | PC7	   | 60  | Cue 8       | OUT       | Digital OUTPUT	| Cue 8 indicator LED                             |
| 31	      | PC6	   | 59  | Cue 10	   | OUT       | Digital OUTPUT	| Cue 10 indicator LED                            |
|             |        |     |             |           |                |                                                 |
| 41	      | PG0	   | 78  | Bar 1       | OUT       | Digital OUTPUT	| Cue 1 indicator LED                             |
| 40	      | PG1	   | 77  | Bar 3       | OUT       | Digital OUTPUT	| Cue 3 indicator LED                             |
| 39	      | PG2	   | 76  | Bar 5       | OUT       | Digital OUTPUT	| Cue 5 indicator LED                             |
| 38	      | PD7	   | 75  | Bar 7       | OUT       | Digital OUTPUT	| Cue 7 indicator LED                             |
| 37	      | PC0	   | 74  | Bar 9       | OUT       | Digital OUTPUT	| Cue 9 indicator LED                             |
| 36	      | PC1	   | 73  | Bar 2       | OUT       | Digital OUTPUT	| Cue 2 indicator LED                             |
| 35	      | PC2	   | 72  | Bar 4       | OUT       | Digital OUTPUT	| Cue 4 indicator LED                             |
| 34	      | PC3	   | 71  | Bar 6       | OUT       | Digital OUTPUT	| Cue 6 indicator LED                             |
| 33	      | PC4	   | 60  | Bar 8       | OUT       | Digital OUTPUT	| Cue 8 indicator LED                             |
| 32	      | PC5	   | 59  | Bar 10	   | OUT       | Digital OUTPUT	| Cue 10 indicator LED                            |
|             |        |     |             |           |                |                                                 |
| 10	      | PD2	   | 78  | Mode 1      | OUT       | Digital OUTPUT	| Mode 1 indicator LED                            |
| 20	      | PD1	   | 77  | Mode 3      | OUT       | Digital OUTPUT	| Mode 2 indicator LED                            |
| 21	      | PD0	   | 76  | Mode 5      | OUT       | Digital OUTPUT	| Mode 3 indicator LED                            |