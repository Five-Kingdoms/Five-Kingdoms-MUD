>greet_prog 50~
if ishome ($i)
	say Get me out of here!
endif
if isindoor ($i)
	tell self shit
else
	disappear
endif
~
>rand_prog 10~
if ishome ($i)
	say Can you show me how to get out of here?
endif
~
>speech_prog p ~
if isnpc($n)
	tell self shit
else
	if isindoor ($i)
		stand
		follow $n
		say Tell me when we get out, I can't see very well.
	else
		setquest $n gnome 1
		say Thank you for rescuing me, $n!
		fol self
		disappear
	endif
endif
~
|

