>greet_prog 100~
if rand (10)
	say Travel well, $N. Many dangers live in these Dragon's Teeth.
endif
if rand (33)
	say These mountains hold some secrets, $N. Best be watchful.
endif
~
>rand_prog 5~
if rand (50)
	pmote walks around the wall, his eyes scanning for trouble.
else
	pmote takes a deep breath of the cool mountain air.
endif
~
>bribe_prog 500~
mpjunk gold
if charobj (2030)
	chown "stones" $n
	mpjunk stones
	load obj 2041
	give fist $n
	say There you go, $n. A deal's a deal.
	setquest $n drgnmtn 1
	else
		say Thanks for the money.
		say You know, if you had some stones...
	endif
endif
~
>bribe_prog 1~
mpjunk gold
say Thanks for the money, but it's not enough for my help.
~
|

