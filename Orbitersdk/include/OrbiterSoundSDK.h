#ifndef __ORBITERSOUNDSDK_H
#define __ORBITERSOUNDSDK_H
/////////////////////////////////////////////////////////////////////////////////////
//       SDK to add sound to Orbiter's VESSEL (c) Daniel Polli  ORBITER ver:021202 //
/////////////////////////////////////////////////////////////////////////////////////
//
// info:
// -----
//
// This SDK allow loading and playing 60 wav sound for vessel.
// Sound are loaded and played throught OrbiterSound.dll so you must
// specify in your doc that the player must have OrbiterSound module installed.
// Several test are done internally to avoid problem if either OrbiterSound
// isn't loaded or the wav doesn't exist etc etc so in this case no sound is played
// and orbiter don't crash.
// You don't need to bother destroying sound they are destroyed automaticly if:
//   -you reload a wav in the same slot (WavNumber)
//   -User exit Orbiter
//
// Usage example:
// --------------
//
// 1. Include OrbitesSoundSDK.h in your vessel's project: #include "OrbiterSoundSDK.h"
// 2. Add OrbiterSoundSDK.lib to your vessel's project
// 3. call ConnectToOrbiterSoundDLL() in your "ovcInit" vessel init routine.
// 4. call RequestLoadVesselWave(1,"Sound\\AtlantisMartin\\MyFirstWave.wav"); //example
// 5. call PlayVesselWave(1) when you want; 
// 6. SEE EXAMPLE.TXT in the orbitersoundSDK's folder.
// et voila !
//
// Function available (see below in the header how to use each function):
// -------------------
// BOOL ConnectToOrbiterSoundDLL(void);
// BOOL RequestLoadVesselWave(int WavNumber,char* SoundName);
// BOOL PlayVesselWave(int WavNumber,int Loop=NOLOOP,int Volume=255,int Frequency=NULL);
// BOOL StopVesselWave(int WavNumber);
// BOOL IsPlaying(int WavNumber);
// BOOL PlayVesselRadioExclusiveWave(int WavNumber,int Loop=NOLOOP,int Volume=255,int Frequency=NULL);
// BOOL IsRadioPlaying(void);
// BOOL ControlAircondFan(BOOL AirCond=TRUE);
// BOOL SoundOptionOnOff(int Option, BOOL Status);
// BOOL SetRadioFrequency(char* Frequency);
//
// IMPORTANT NOTE:
// ---------------
//
// Sound Folder !!!  Keep it clear for player
// ------------------------------------------
// For consistency I think it's better to put your sound in the OrbiterSound's directory
// and create your specific folder here with a UNIQUE name so vessel sound add-on
// will not overlap and user will not end with dozen of sound folder trowed everywhere
// in its Orbiter's directory.
//
// Example:
//   if you add sound for Atlantis and your name is Martin
//   create a directory  "Orbiter/Sound/AtlantisMartin/"
//   put your wav in this folder and at load time call:
//   RequestLoadVesselWave(1,"Sound\\AtlantisMartin\\MyFirstWave.wav");
//
//   otherwise if you and another vessel creator choose the same common name for example
//   "atlantis" your or his wav will be erased depend on wich add-on is installed 
//   later.
//
// Sliding sound
// --------------
// if you call each game loop a PlayVesselWave function while changing its parameter
// the sound will continue to play with the new parameter
// so you can make sound that fade in , fade out or slide in frequency.
//
// Wave Format
// -----------
// Orbiter sound is locked at 22050 hz for performance care so it's useless
// to load sound that have higher frequency.
// For size's care I suggest a format of 11025 8 bit mono for radio and other minor sound
// and more until 22050 for sound that require higher quality. Stereo play well also
// but they require double size. Try to keep size as low as possible otherwise loading
// time will become very long.
// WARNING: take care that your wave doesn't have an ADPCM format the old compressed format
// this one will not play on most computer due to codec. 
// instead always Use PCM the default windows format.
//
//
////////////////////////////////////////////////////////////////////////////////

// warn if forgetting to add OrbiterSoundSDK.lib file
#pragma comment(lib,"OrbiterSoundSDK.lib")

// WAVENUMBER HELPER
// -----------------
// You can change the name defined below
// for the one that recall your sound's name
// EXAMPLE :  
//  #define MYGEARSOUND		     1
//  #define ALT10000SOUND        2
//  #define SEPARATESRB          3
//  ....
// This have no other purpose then keeping
// your code readable. It will be also better to declare them directly
// in you code to not change OrbiterSoundSDK's header. (see example in folder VesselSDK)
#define VESSELSOUND1	1
#define VESSELSOUND2	2
#define VESSELSOUND3	3
#define VESSELSOUND4	4
#define VESSELSOUND5	5
#define VESSELSOUND6	6
#define VESSELSOUND7	7
#define VESSELSOUND8	8
#define VESSELSOUND9	9
#define VESSELSOUND10	10
#define VESSELSOUND11	11
#define VESSELSOUND12	12
#define VESSELSOUND13	13
#define VESSELSOUND14	14
#define VESSELSOUND15	15
#define VESSELSOUND16	16
#define VESSELSOUND17	17
#define VESSELSOUND18	18
#define VESSELSOUND19	19
#define VESSELSOUND20	20
#define VESSELSOUND21	21
#define VESSELSOUND22	22
#define VESSELSOUND23	23
#define VESSELSOUND24	24
#define VESSELSOUND25	25
#define VESSELSOUND26	26
#define VESSELSOUND27	27
#define VESSELSOUND28	28
#define VESSELSOUND29	29
#define VESSELSOUND30	30
#define VESSELSOUND31	31
#define VESSELSOUND32	32
#define VESSELSOUND33	33
#define VESSELSOUND34	34
#define VESSELSOUND35	35
#define VESSELSOUND36	36
#define VESSELSOUND37	37
#define VESSELSOUND38	38
#define VESSELSOUND39	39
#define VESSELSOUND40	40
#define VESSELSOUND41	41
#define VESSELSOUND42	42
#define VESSELSOUND43	43
#define VESSELSOUND44	44
#define VESSELSOUND45	45
#define VESSELSOUND46	46
#define VESSELSOUND47	47
#define VESSELSOUND48	48
#define VESSELSOUND49	49
#define VESSELSOUND50	50
#define VESSELSOUND51	51
#define VESSELSOUND52	52
#define VESSELSOUND53	53
#define VESSELSOUND54	54
#define VESSELSOUND55	55
#define VESSELSOUND56	56
#define VESSELSOUND57	57
#define VESSELSOUND58	58
#define VESSELSOUND59	59
#define VESSELSOUND60	60
// keyword for looped sound
// to use with PlayVesselWave
#define NOLOOP			0
#define LOOP			1

// This is the keyword used in SoundOptionOnOff()
// look below at this function to see how to use them
#define PLAYCOUNTDOWNWHENTAKEOFF	 1		// the countdown of apollo11 when you take off
#define PLAYMFDCOUNTDOWN			 2		// the countdown sound when you use the countdown MFD (SHF-W)
#define PLAYWHENATTITUDEMODECHANGE	 3		// play "rotation" "linear" sound when you change mode
#define PLAYGPWS					 4		// the GPWS sound
#define PLAYMAINTHRUST				 5		// main thrust sound
#define PLAYHOVERTHRUST				 6		// the hover thrust sound
#define PLAYATTITUDETHRUST			 7		// the attitude thrust sound
#define PLAYDOCKINGSOUND			 8		// the docking sound and radio
#define PLAYRADARBIP				 9		// the radar bip when near another vessel
#define PLAYWINDAIRSPEED			 10		// the wind airspeed when atmosphere
#define PLAYDOCKLANDCLEARANCE		 11		// the landing clearance you can hear bellow 3000m or near a station
#define PLAYLANDINGANDGROUNDSOUND	 12		// Rolling , landing, speedbrake, crash sound
#define PLAYCABINAIRCONDITIONING	 13		// play the air conditionning sound
#define PLAYCABINRANDOMAMBIANCE		 14		// play the random pump and rumble sound
#define PLAYWINDAMBIANCEWHENLANDED	 15		// play the wind sound when landed
#define PLAYRADIOATC				 16		// play the atc radio sound
#define DISPLAYTIMER				 17		// display the timer text at the bottom of the screen when you take-off
#define DISABLEAUTOPILOTWHENTIMEWARP 18		// the auto-disable of pilot if you time warp (avoid crash in orbiter)
#define ALLOWRADIOBLACKOUT			 19		// Allow the radio blackout when reentry
#define MUTEORBITERSOUND			 20		// Mute all the OrbiterSound except the VesselSDK one

///////////////////////////////////////////////////
// ConnectToOrbiterSoundDLL
// ------------------------
// this must be called previous to any other call
// "ovcInit" function is perfect for this
// if it return FALSE it will mostly mean that
// OrbiterSound's module isn't loaded.
// in this case all the other function will return
// FALSE and do nothing so Orbiter will not crash. 
///////////////////////////////////////////////////
BOOL ConnectToOrbiterSoundDLL(void);

///////////////////////////////////////////////////////////////////////////////////////
// RequestLoadVesselWave
// --------------------
// Request to Load one sound in slot "WavNumber"
//
// param:
// ------
// WavNumber: from 1 to 60 or #defined name from 1 to 60
// SoundName: your wav's name and path. example: "Sound\\atlantisMartin\\separate.wav"
//
// return:
//  FALSE if request not taken in account.
//  if a wave wasn't found a log will be created in Orbiter's directory with the name of the 
//  missing wave
//
// Note:
// -----
// When you call this function THE WAV ISN'T LOADED DIRECTLY
// due to the fact that the vessel is initialised before directsound when orbiter start,
// this function SUBMIT the loading. As soon as directsound is ready it will load
// the sound. The delay is due while Orbiter open its screen at last when all
// is initialised and DirectSound need the screen's handle previous to be initialised.
// So the sound will be loaded at least when Orbiter screen open Else it's almost 
// instantly. the best place to load wave is "ovcinit" function just after 
// ConnectToOrbiterSoundDLL() function.
//
// You can submit a new loading at same "slot" any time during simulation the new
// one will erase the old one. Take care not to ask a loading every frame otherwise
// the framerate will suffer. Take car that if the user want to save it's situation
// and come back later you may define some flag and record the time so when the player
// come back you reload the appropriate wav for the time it is.
// 
// you can use plain WavNumber from 1 to 60 or use the name
// defined above for more readability.
// ie: it's more clear to call:
// LoadVesselWave(SEPARATESRB,"Sound\\atlantisMartin\\separate.wav");
// PlayVesseWave(SEPARATESRB);
//   than
// LoadVesselWave(18,"Sound\\atlantisMartin\\separate.wav");
// PlayVesseWave(18);
// when you end with dozen line of code you will wonder what playvesselwave(18) will do.
//
// if you plan to reload some sound at the same slot during simulation you can also
// declare another set of name.
// Ie:
// #define LAUNCHONEARTHSOUND    0   // set number one in slot 0
// ...
// #define LAUNCHONMOONSOUND     0   // set number two in slot 0 (erase the previous one if loaded)
////////////////////////////////////////////////////////////////////////////////////////
BOOL RequestLoadVesselWave(int WavNumber,char* SoundName);

///////////////////////////////////////////////
// PlayVesselWave
// --------------
// Play a wave loaded before
//
// param:
// ------
// WavNumber: from 1 to 60 or #defined name from 1 to 60
// Loop		: NOLOOP= sound not looped LOOP= sound looped.
// Volume	: from 0 to 255
// Frequency: from 3000 to 44100  (OrbiterSound's play anyway at 22050 so your sound will be resampled at run time)
//
// return:
//  FALSE if wave not played/loaded OR the connection to orbitersound's dll failed.
//
// example:
// --------
//  PlayVesselWave(SEPARATESRB);			// play with no loop full volume and wav's default frequency
//  PlayVesselWave(SEPARATESRB,LOOP,210);	// play a looped sound with specified volume and wav's default frequency
//  PlayVesselWave(SEPARATESRB,NOLOOP,128,22050);// play no loop with specified volume and frequency
///////////////////////////////////////////////
BOOL PlayVesselWave(int WavNumber,int Loop=NOLOOP,int Volume=255,int Frequency=NULL);

///////////////////////////////////////////////
// StopVesselWave
// --------------
// Stop a wave that play
//
// param:
// ------
// WavNumber: from 1 to 60 or #defined name from 1 to 60
//
// return:
//  FALSE if connection to orbitersound's dll failed or sound not loaded.
//
// example:
// --------
//  StopVesselWave(SEPARATESRB);		
///////////////////////////////////////////////
BOOL StopVesselWave(int WavNumber);

///////////////////////////////////////////////
// IsPlaying
// ---------
// Return TRUE if the sound specified in WaveNumber
// is currently playing otherwise it return FALSE.
//
// param:
// ------
// WavNumber: from 1 to 60 or #defined name from 1 to 60
//
// return:
// -------
//  FALSE if wave does not play OR the connection to orbitersound's dll failed.
//
// this is useful to avoid playing two sound in same time
// example:
// --------
//  if(IsPlaying(SEPARATESRB)==FALSE)
//  {
//     PlayVesselWave(AFTERSRBSEPARATE);		
//  }
///////////////////////////////////////////////
BOOL IsPlaying(int WavNumber);

///////////////////////////////////////////////
// PlayVesselRadioExclusiveWave
// ----------------------------
// Work exactly as PlayVesselWave except that it:
//
// 1- STOP the ATC RADIO sound that OrbiterSound is currently playing
// 2- FORBID OrbiterSound to play radio atc while this sound is playing. (auto test)
// as soon as your sound end OrbiterSound will continue to play random ATC radio Wave
// this is perfect to play your radio sound or important sound without being disturbed by
// OrbiterSound's atc sound.
//
// For parameter see PlayVesselWave
//
// Note:
// it is perhaps better to test with IsRadioPlaying() if an atc wav is currently playing
// before calling this or the Orbiter's atc sound will stop in the middle and that may
// look strange.
// Try to avoid LOOP parameter or no more atc sound will be heard from OrbiterSound.
//
///////////////////////////////////////////////
BOOL PlayVesselRadioExclusiveWave(int WavNumber,int Loop=NOLOOP,int Volume=255,int Frequency=NULL);

///////////////////////////////////////////////
// IsRadioPlaying
// --------------
// Return TRUE if an OrbiterSound's atc radio wav is currently playing
// otherwise it return FALSE.
// This is usefull to avoid playing YOUR radio wav during
// and OrbiterSound's atc one.
//
// return:
//  FALSE if radio atc does not play OR the connection to orbitersound's dll failed.
//
// example:
// --------
//  if(IsRadioPlaying()==FALSE)
//  {
//     PlayVesselWave(MYRADIODOCKED);		
//  }
///////////////////////////////////////////////
BOOL IsRadioPlaying(void);

///////////////////////////////////////////////
// ControlAircondFan
// -----------------
// This control the Air conditionned fan of OrbiterSound
// If you set it to FALSE the sound will slowly shutdown
// Set it to TRUE to restart it.
// Notice that if player set focus to another vessel the sound will be reseted to TRUE
//
// return:
//  FALSE if the connection to orbitersound's dll failed.
//
// example:
// --------
//  ControlAircondFan(FALSE);		// Shutdown the sound
//  ControlAircondFan(TRUE);		// Restart the sound
///////////////////////////////////////////////
BOOL ControlAircondFan(BOOL AirCond=TRUE);

///////////////////////////////////////////////
// SoundOptionOnOff
// -----------------
// With this function you can put ON/OFF every sound
// in OrbiterSound the same way SOUNDCONFIG.EXE does
// but in a temporary manner. 
// (start soundconfig.exe to test the sound related to each option so you have an idea what it does)
// 
// param:
// ------
// Option: Keyword as defined below:
//
// PLAYCOUNTDOWNWHENTAKEOFF			// the countdown of apollo11 when you take off
// PLAYMFDCOUNTDOWN					// the countdown sound when you use the countdown MFD (SHF-W)
// PLAYWHENATTITUDEMODECHANGE		// play "rotation" "linear" sound when you change mode
// PLAYGPWS							// the GPWS sound
// PLAYMAINTHRUST					// main thrust sound
// PLAYHOVERTHRUST					// the hover thrust sound
// PLAYATTITUDETHRUST				// the attitude thrust sound
// PLAYDOCKINGSOUND					// the docking sound and radio
// PLAYRADARBIP						// the radar bip when near another vessel
// PLAYWINDAIRSPEED					// the wind airspeed when atmosphere
// PLAYDOCKLANDCLEARANCE			// the landing clearance you can hear bellow 3000m or near a station
// PLAYLANDINGANDGROUNDSOUND		// Rolling , landing, speedbrake, crash sound
// PLAYCABINAIRCONDITIONING			// play the air conditionning sound
// PLAYCABINRANDOMAMBIANCE			// play the random pump and rumble sound
// PLAYWINDAMBIANCEWHENLANDED		// play the wind sound when landed
// PLAYRADIOATC						// play the atc radio sound
// DISPLAYTIMER						// display the timer text at the bottom of the screen when you take-off
// DISABLEAUTOPILOTWHENTIMEWARP		// the auto-disable of pilot if you time warp (avoid crash in orbiter)
// ALLOWRADIOBLACKOUT			 	// Allow the radio blackout when reentry
// MUTEORBITERSOUND			 		// Mute all the OrbiterSound except the VesselSDK one
//
// Status: TRUE if you want to allow the sound FALSE if you want to disable it (sound stop immediately when you call it)
//
// return:
//  FALSE if the connection to orbitersound's dll failed
//  or the keyword used is false
//
// example:
// --------
//  SoundOptionOnOff(PLAYMAINTHRUST,FALSE)		// Disable the main thrust sound in orbitersound
//  SoundOptionOnOff(PLAYGPWS,TRUE)				// Enable the GPWS sound in OrbiterSound
///////////////////////////////////////////////
BOOL SoundOptionOnOff(int Option, BOOL Status=TRUE);

///////////////////////////////////////////////
// SetRadioFrequency
// -----------------
// With this function you can set the main radio frequency 
// used in OrbiterSound.
// 
// param:
// ------
// frequency: a char string in the form "nnn.nnn"
// a directory with the same name must exist in the sound/radio folder
//
// return:
//  FALSE if the connection to orbitersound's dll failed
//
// example:
// --------
//  SetRadioFrequency("296.800");		// Set the frequency 296.800. radio will play the sound contained in this folder
///////////////////////////////////////////////
BOOL SetRadioFrequency(char* Frequency);

#endif 
