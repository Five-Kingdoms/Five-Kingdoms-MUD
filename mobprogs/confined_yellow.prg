>greet_prog 100~
if rand (100)
	say I am the Yellow Oracle. 
	say To pass you must hold the Yellow Crystal Shard.
endif
if charobj (9592)
	say You hold the first key to the five portals.
	say With this equipped, so shall you pass.
	echo `#A yellow crystal shard glows brightly!``
	chown "yellow crystal" $n
	load obj 9335
	give crystal $n
	say Passage is yours.
	else
		say You come ill equipped for this task...
		say Return with the yellow crystal shard.
		gteleport $n 9331
	endif
endif
~
|

