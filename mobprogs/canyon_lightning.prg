>give_prog 100~
if number($o) == 9254
        say Thank you for returning my amulet.
        setquest2 $n canyon 8
        mpjunk $o
endif
~
>greet_prog 5~
if charobj (9254)
	say Ahh..  You have found my amulet, $N.
endif
~
>rand_prog 3~
if rand(50)
	say Nothing is as quick as lightning.
else
	say Nothing can destroy lightning.
endif
~
|

