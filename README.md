# gemini4orbiter
Continuation of the old Gemini Titan 2 add-on, Version 4.5 for Orbiter 2016 or later

# Scope

The scope of this project shall be limited to the following work until Walpurgis Night 2022. Then a decision about the scope for the following year shall be made.

## The Don'ts:

* No detailled subsystem simulations (yet). The GDC emulation code I included will be kept for reference, but I will not work on it.
* No big prelaunch campaign. Everything shall start maximal 24 minutes before L-0.
* No switch-flipping simulation. If you need to set switches into one specific configuration for progressing to the next mission phase, a single key or mouse press is also enough.
* No MOL or Gemini B or other further big developments in the first year.
* No rogallo wing.
* No pilot position controls.
* No sandwichs.

## The Maybe's:

* Simple implementation of the GDC for driving the IVI and ADI.
* Launch simulations of GATV or ATDA shall be strictly optional and can be done later in the year.
* Recovery fleet and vehicles might be included in a simplified way to end a mission with style.
* The unmanned Gemini 1 and Gemini 2 missions could be included.
* Lunar Gemini can be included, since it is only a minor change compared to the GATV.
* If possible, there shall be virtual Gemini development conference on the 1st of May, 2022 to decide the scope for the next year.
* Include nicer looking animated launch complexes.
* Better looking Titan 2 staging, include the fireball reported by the astronauts.
* A simplified crew simulation, focus on health.
* Pilot side indicators.
* Pilot view only during EVA.
* A plotting board with map.
* Limited support from a virtual mission control:
   * Update time to reentry
   * Control GATV
   * Update launch time and trajectory.
* Show the construction of the Apollo infrastructure during the Gemini era by using multiple configurations of Cape Canaveral, using the PERIOD parameter.

## The Do's:

* I like VCs. I want a VC (Even if its just one cockpit configuration and not the accurate ones)
* I want to split the Gemini spacecraft from the GLV. Mostly for preparation for later developments (using different launchers), but also keeping the complexity of the vessel a bit lower.
* The IVI shall be functional, even if only manual input of dVs.
* I want to automatize acceptance testing, so the dev velocity can increase from release to release and I can realize more interesting things.
* All abort sequences should be possible, including using ejection seats.
* I want tools to analyse bugs that happen at the player side beyond log files.
   * Support Orbiter's flight recorder
   * A ring buffer with the final 10 or 30 seconds of telemetry (30s = 192 kB binary file).
* The horizon scanners shall finally be animated and shiny.
* The sequential system logic shall be accurate in its behaviour.
* A simplified electronic timer shall be there to count GET and TR. TX is optional right now.
* Changing/reading TR during flight shall be possible via a vessel-specific MFD.
* The backend framework shall be ready to support complex subsystem simulations 
   * Loading/Saving of important subsystem parameters shall be automatic.
   * Test framework support, asserting subsystem states.

