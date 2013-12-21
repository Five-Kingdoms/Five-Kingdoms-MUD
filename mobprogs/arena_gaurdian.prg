>greet_prog 100~
if islevel($n) > 40
say Only warrior classes of rank 35 to 40 may enter here.
endif

if islevel($n) < 35
say Only warrior classes of rank 35 to 40 may enter here.
endif

if isclass($n) == 0    
say Hail, $n.
unlock east
open east
say Welcome to the Arena!
nod $n
else
	if isclass($n) == 1
	say Hail, $n.
	unlock e
	open e
	say Welcome to the Arena!
	nod $n
	else
		if isclass($n) == 2
		say Hail, $n.
		unlock e
		open e
		say Welcome to the Arena!
		nod $n
		else
			if isclass($n) == 5
			say Hail, $n.
			unlock e
			open e
			say Welcome to the Arena!
			nod $n.
			else
				if isclass($n) == 6
				say Hail, $n.
				unlock e
				open e
				say Welcome to the Arena!
				nod $n
				else
					if isclass($n) == 8
					say Hail, $n.
					unlock e
					open e
					say Welcome to the Arena!
					nod $n
					else
						say You are not welcome here, $n.
					endif
				endif	
			    endif
			endif
		endif
	endif
endif
~

>bloody_prog 100~
say You are too bloody to enter this Arena, $n!
say Do not bring your personal battles to the Arena, $n!
pmote blocks $n from entering the Arena.
close east
~

>rand_prog 15~
if rand(10)
	pmote adjusts his armor, and looks about for anything to slay.
else
	pmote nods at you, and continues his watch over the Arena.
endif
~
|
	

