>greet_prog 100~
if rand (100)
	say I am the Green Oracle. 
	say To pass you must hold the Green Crystal Shard.
endif
if charobj (20726)
	say You hold the first key to the five portals.
	say With this equipped, so shall you pass.
	echo `@A green crystal shard glows brightly!``
	chown "green crystal" $n
	load obj 9334
	give crystal $n
	say Passage is yours.
	else
		say You come ill equipped for this task...
		say Return with the green crystal shard.
		gteleport $n 9331
	endif
endif
~
|

