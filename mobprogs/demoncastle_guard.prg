>greet_prog 100~
if israce($n) == 28    
say Hail, Demon $n.
load obj 27015
unlock east
open east
say Go in Chaos.
mpjunk key
else
	if iscabal($n) == 10
	say Greetings, $n. Welcome to the Dark Castle.
	nod $n
	else
		if iscabal($n) == 12
		yell Savants are intruding upon the Dark Castle!
		say No trespassing, $n!
		murder $n
		else
			if iscabal($n) == 3
			scream $n
			yell Justice is invading the castle!
			say Die, fool!
			murder $n
			say Order is not welcome here!
			else
				if iscabal($n) == 1
				scream $n
				say Die, $n, you foolish Knight!
				yell Knights are invading the castle!
				murder $n
				say Die in your Light!
				else
					murder $n
				endif	
			    endif
			endif
		endif
	endif
endif
~

>bloody_prog 100~
say You are too bloody to enter this castle, $n!
say Do not bring your battles to the castle, $n!
pmote blocks $n from entering the drawbridge.
close east
~

>rand_prog 15~
if rand(10)
	pmote adjusts his armor, and looks about for anything to slay.
else
	pmote nods at you, and continues his watch over the castle.
endif
~
|
	

