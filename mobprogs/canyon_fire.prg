>give_prog 100~
if number($o) == 9252
        say Thank you for returning my amulet.
        setquest2 $n canyon 2
        mpjunk $o
endif
~
>greet_prog 5~
if charobj (9252)
	say Ahh..  You have found my amulet, $N.
endif
~
>rand_prog 3~
if rand(50)
	say Nothing is as useful or dangerous as fire.
else
	say How would you like to be burned alive?
endif
~
|

