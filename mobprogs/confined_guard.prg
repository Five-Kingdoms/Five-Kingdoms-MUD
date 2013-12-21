>greet_prog 100~
if rand (100)
	say The Nexus, you now stand within. 
	say If you are prepared, you will soon delve deeper.
	say If you are not, here, you will remain.
endif
if charobj (10850)
	say To find the just, you must wear the Breatsplate of Truth.
	say With this equipped, so shall you pass.
	if charobj (10625)
	say To see the light, you must bear the Fullhelm of Blind Justice.
	say With this equipped, so shall you pass.
		if charobj (3468)
		say Only with the Will of Gods, will you make it out alive.
		say With this equipped, so shall you pass.
		say You are prepared...
		say You cast aside your mortal coil, and sumbit to the Nexus.
		say May the Gods have mercy on you and your group.
		gteleport $n 9300
	else
		say You come ill equipped for this task...
		say Return with Truth, Blind Justice, and the Will of Gods...
		say Lest you be destroyed for eternity...
		gteleport $n 9332
	endif
     endif
   endif
endif
~
|

