>greet_prog 100~
if rand (100)
	say I am the Red Oracle. 
	say To pass you must hold the Red Crystal Shard.
endif
if charobj (1126)
	say You hold the first key to the five portals.
	say With this equipped, so shall you pass.
	echo `!A red crystal shard glows brightly!``
	chown "red crystal" $n
	load obj 9333
	give crystal $n
	say Passage is yours.
	else
		say You come ill equipped for this task...
		say Return with the red crystal shard.
		gteleport $n 9331
	endif
endif
~
|

