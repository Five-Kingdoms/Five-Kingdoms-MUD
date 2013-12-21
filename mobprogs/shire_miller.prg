>rand_prog 3~
if rand(50)
	say Stop slacking off!
else
	say Worker harder.
endif
~
>greet_prog 5~
if isnpc ($r)
	tell self shit
else
	if israce ($n) == 6
		say What?  Do you want a job or something?
	else
		say Welcome to the Watermill  Would you like a tour?
	endif
endif
~
|

