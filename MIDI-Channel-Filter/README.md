# MIDI Channel Filter
This MIDI-effect can filter out selected MIDI messages, passing only those sent on a specified MIDI channel. All other MIDI messages pass through. It can also filter out any combination of MIDI channels entirely.

### User Interface

The graphical user interface (GUI) consists of two labeled groups of controls:

![screenshot](MIDI-Channel-Filter.png)

#### Program/Bank Filter

The controls in the "Program/Bank Filter" group at the top allow filtering MIDI program-change and/or bank-select messages on one selected channel. The blue knob selects the channel, and the three checkboxes select which types of messages are to be blocked on that channel:

- Program Change
- Bank-Select MSB (CC#0)
- Bank-Select LSB (CC#32)

Each message type is only filtered if the corresponding checkbox is checked; otherwise it is allowed through. For example, if only *Program Change* is checked and *MIDI Channel* is set to 5:

- Incoming MIDI program-change messages will only pass through if they are sent on MIDI channel 5; program-change messages on any other channel will be blocked.
- All other MIDI messages (sent on any MIDI channel) will pass through.

#### MIDI channels to block entirely

The lower group of checkboxes allows you to select any combination of MIDI channels which are to be blocked entirely. For example, if box 7 is checked, ALL incoming MIDI messages on that channel will be blocked.

If you have any of the boxes in the "Program/Bank Filter" group checked, make sure not to check the box for the channel you chose using the Channel knob. If you only want to do whole-channel blocking, un-check all three boxes in the top group.

### Parameters

- **MIDI Channel** may be set to 1 through 16. This is the channel-number which selected messages must match in order to be allowed through.
- **Program Change**, **Bank Select MSB**, and **Bank Select LSB** are Boolean parameters, which correspond to the three checkboxes in the GUI. If the parameter value is greater than or equal to 0.5, the checkbox will be checked, and the selected MIDI messages will be filtered based on the selected MIDI channel.
- **Block Channel 1** through **Block Channel 16** are Boolean parameters, corresponding to the 16 checkboxes in the "MIDI channels to block entirely" group.

