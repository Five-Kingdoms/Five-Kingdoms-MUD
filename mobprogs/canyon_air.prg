>give_prog 100~
if number($o) == 9253
        say Thank you for returning my amulet.
        setquest2 $n canyon 4
        mpjunk $o
endif
~
>greet_prog 5~
if charobj (9253)
	say Ahh..  You have found my amulet, $N.
endif
~
>rand_prog 3~
if rand(50)
	say Nothing can withstand the mighty power of the wind.
else
	say Try breathing without air.
	cackle
endif
~
|

