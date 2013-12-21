>give_prog 100~
if number($o) == 9255
        say Thank you for returning my amulet.
        setquest2 $n canyon 16
        mpjunk $o
endif
~
>greet_prog 5~
if charobj (9255)
	say Ahh..  You have found my amulet, $N.
endif
~
>rand_prog 3~
if rand(50)
	say You would die without water.
else
	say Nothing has as many forms as water.
endif
~
|

