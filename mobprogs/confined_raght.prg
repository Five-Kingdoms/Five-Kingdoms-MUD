>greet_prog 100~
if rand (100)
	say Justice be praised, $n, you have come!
	say I do hope Falthar has been freed...
	say Crypticant comes for me. 
	say Even now I can feel his hot breath in the darkness.
	emote tightens his grip around his sword.
	say I must have those crystal triads...
	say It is our only hope.
	chown "red crystal" $n
	chown "blue crystal" $n
	chown "green crystal" $n
	chown "black crystal" $n
	chown "yellow crystal" $n
	emote fastens the crystals to the hilt of his sword!
	emote has created '`!Nagim-Dur``' the `&Sword`` of `6God``!
	say I must get your out of here, $n.
	say Take this sphere, and give it to Streantian.
	say He will see that it is used to fruition.
	load obj 9337
	give sphere $n
	emote readies himself for battle.
        setquest $n justice 1
	gteleport $n 77
endif
~
|

