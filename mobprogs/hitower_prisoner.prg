>greet_prog 50~
if inroom ($i) == 1300
	tell self shit
else
	if inroom ($i) < 1500
		say Please get me out of here.
	endif
endif
~
>rand_prog 10~
if inroom ($i) == 1300
	tell self shit
else
	if inroom ($i) < 1500
		say I've been imprisoned for finding out too many secrets.
	endif
endif
~
>speech_prog p follow~
if inroom ($i) == 1300
	tell self shit
else
	if inroom ($i) < 1500
		stand
		follow $n
	endif
endif
~
>entry_prog 100~
if inroom ($i) == 1300
	say Thank you for rescuing me.
	fol self
endif
if inroom ($i) != 1300
	tell self shit
else
	if rand(33)
		say The master enchanter has a very special dagger.
	else
		if rand(50)
			say The battle mistress has a very special dagger.
		else
			say The master of goodness has a very special dagger.
		endif
	endif
endif
~
|

