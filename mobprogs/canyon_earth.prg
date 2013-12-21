>give_prog 100~
if number($o) == 9251
	say Thank you for returning my amulet.
	setquest2 $n canyon 1
        mpjunk $o
endif
~
>greet_prog 5~
if charobj (9251)
	say Ahh..  You have found my amulet, $N.
else
	say If you find my amulet, the power of earth shall be yours.
endif
~
>rand_prog 3~
if rand(50)
	say Nothing is as durable as earth.
else
	say The destruction of earthquakes and mudslides is unparalleled.
endif
~
|

