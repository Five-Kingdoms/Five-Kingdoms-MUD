>greet_prog 100~
if rand (100)
	say I am the Black Oracle. 
	say To pass you must hold the Black Crystal Shard.
endif
if charobj (6008)
	say You hold the first key to the five portals.
	say With this equipped, so shall you pass.
	echo `8A black crystal shard glows brightly!``
	chown "black crystal" $n
	load obj 9336
	give crystal $n
	say Passage is yours.
	else
		say You come ill equipped for this task...
		say Return with the black crystal shard.
		gteleport $n 9331
	endif
endif
~
|

