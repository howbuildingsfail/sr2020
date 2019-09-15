# sr2020
an sr-88 drum machine for 2020


# sr2020 execution pseudocode

The `loop()` function should contain the following elements: 

```
if(playing)
	triggerActiveVoices()
	wait(triggerLength)

turnOffAllTriggers()

updateTempo()
updateCurrentMemory(variation)
updatePlayRecMode()
updatePlaying()

if(playing)
	wait(beatLength)


```



# SR-88 Controls

- **vol/power** We'll need these - vol will be on the analogue out, so won't need arduino links, but an off/on switch is always nice.
- **Tone** We won't need this becuase there'll be various analogue controls on each voice
- **Tempo** - this will need an analogRead pin
- **Variation** - needs some thought: A, B, AB and Fill-in seem a bit limited and there are many other possibilities we might do including random switching
- **Sound** - Should be able to manage this with a button in combination with OLED display
- **Mode** - Play/Rec - I want to add in 'live' input instead of the usual step write - but this should be possible in combination with the start/stop button
- **Start/Stop** - As here but with addition of 'live' input
- **Memory** - Need as many as we can fit on an EEPROM - use two pushbuttons instead of a rotary switch in combination with the EEPROM

## Sockets

- **AC Adaptor**
- **Clock**
- **SQ**
- **Out**
- **Footsw**