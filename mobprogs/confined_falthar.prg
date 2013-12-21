>greet_prog 100~
if rand (100)
	say Justice be praised, $n, you have come!
	say I fear the Demons are close...I will need your powers!
	chown "breastplate" $n
	chown "fullhelm" $n
	chown "will" $n
	wear all
	say I can fight Malignant now...
	say And I must get you out of here!
	say Take this stone, and give it to Streantian...
	load obj 9338
	give stone $n
	emote readies himself for battle.
	say It is time to kill a Demon.
	say Now you go find Raght! Hurry!
	gteleport $n 9303
endif
~
|

