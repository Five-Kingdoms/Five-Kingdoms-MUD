>greet_prog 100~
if rand (100)
	say I am the Blue Oracle. 
	say To pass you must hold the Blue Crystal Shard.
endif
if charobj (1125)
	say You hold the first key to the five portals.
	say With this equipped, so shall you pass.
	echo `$A blue crystal shard glows brightly!``
	chown "blue crystal" $n
	load obj 9332
	give crystal $n
	say Passage is yours.
	else
		say You come ill equipped for this task...
		say Return with the blue crystal shard.
		gteleport $n 9331
	endif
endif
~
|

