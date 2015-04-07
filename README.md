Pokémon Emerald Version 2
=========================

There's still (very) much to do, but when it's complete, it's a recode of Pokémon for Nintendo DS.
As the game itself is in German, I won't go into detail about the (planned) storyline (at least in the English description);
so far the game is set in the Hoenn region 2 years after [Pokémon Ruby, Sapphire](https://en.wikipedia.org/wiki/Pok%C3%A9mon_Ruby_and_Sapphire) and [Emerald](https://en.wikipedia.org/wiki/Pok%C3%A9mon_Emerald).

This game is intended to be played on real hardware, but [DeSmuME 0.9.10](http://desmume.org/) seems to be capable of simulating most of real hardware's features, too.

Features
--------

Features implemented in a non-master branch are in _italics_.

* [ ] A fully working Pokémon engine, including
    * [x] The ability to display a Pokémon's status, _including detailed information about the Pokémon's moves and ribbons_
    * [ ] An in-game Pokémon storage system
    * [x] A fully working PokéDex, loading its data from the ROM's filesystem _and displaying the various formes of the Pokémon_
    * [ ] A fully working battle engine, capable of simulaitng single and double battles
    * [x] The ability to load maps (for now only GBA-style maps are supported, 3D maps are planned) including
      * [ ] Events
      * [ ] Wild Pokémon encounter
      * [x] Animated map tiles
      * [x] Player sprite, animated moving and surfing
    * [ ] Playing .midi as BGM, user customization of BGM via music stored on the micro SD card
    * [ ] RTC, including overword changes based on time
    * [ ] Saving the progress to the micro SD
    * [ ] Communicating with “the originals”
      * [x] Playing with a save from a GBA version (saving is still not implemented yet, though)
      * [ ] Link trading with Gen IV and Gen V games
    * [x] An almost up-to-date UI
* [ ] Easy-to-understand and ready-to-(re)use code
    * [x] Strict and intuitive naming conventions
    * [ ] Good documentation
    * [x] _Modular code (strict seperation of each feature and between core and UI)_

Some screens
------------

##### General gameplay
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_09_9809.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_44_9923.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_07_1429.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_46_10126.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_01_27521.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_09_10201.png)

##### Pokémon status screens

![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_10_10008.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_13_11915.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_02_11879.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_29_10070.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_51_11843.png)

##### The PokéDex
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_01_10175.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_39_27519.png)

##### The Bag
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_01_30165.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_54_30142.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_08_30188.png)

##### Battles
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_23_27593.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_21_25627.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_32_25663.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_40_25689.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_52_25728.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_00_25754.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_06_25774.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_17_25810.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_34_25865.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_57_25940.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_42_26087.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_19_26208.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_18_26597.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_26_26623.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/P-Emerald_2_01_27913.png)

_More to come_, as this README.md is still under construction!
