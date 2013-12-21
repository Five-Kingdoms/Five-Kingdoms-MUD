>greet_prog 10~
if isnpc($n)
	tell self shit
else
	if israce($n) == 4
		say Come to pick up your child, $N?
	endif
endif
~
>speech_prog p Shh.. I'm supposed to be asleep.~
say Go to sleep!
~
>speech_prog p Okay, good night, nanny.~
pmote tucks the dwarven youth in.
~
>rand_prog 3~
pmote runs around taking care of the children.
~
|

