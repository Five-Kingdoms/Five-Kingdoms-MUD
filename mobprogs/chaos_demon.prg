>greet_prog 100~
if charobj (30055)
	pmote glares at you with fiery eyes and snarls!
	say `!Chaos awaits you, $N.``
	pmote escorts you up a narrow flight of stairs.
	say `!Wait here.``
	teleport $n 30100 
else
	yell Die $n! Your corpse will be my gift to the Dark Lord!.
	murder $n
endif
~
>rand_prog 2~
pmote peers around $k, balefully keeping guard.
~
>fight_prog 50~
if rand(33)
        yell $n is attempting to enter the Tower of the Winds!
else
        if rand(10)
                yell You shall never leave this tower alive, $n!
        else
                if rand(20)
                        say `!And so shall your bones burn, $n!``
                	cast 'flamestrike'
		else
			say `!Feel the arcane powers of```1 Hell```!, $n!``
                        cast 'hellstream'
                endif
        endif
endif
~
|
