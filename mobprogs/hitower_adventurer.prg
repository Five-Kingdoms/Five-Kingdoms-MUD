>greet_prog 50~
if inroom ($i) == 1300
	tell self shit
else
	if inroom ($i) < 1500
		say If you get me out of here, I'll tell you something I've learned.
	endif
endif
~
>rand_prog 10~
if inroom ($i) == 1300
	tell self shit
else
	if inroom ($i) < 1500
		say I'll tell you a secret if you get me out of here.
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
	say Thank you for getting me out of there.
	fol self
	say Well, I suppose you want to know the information I have.
	say It seems that there are a couple daggers in the High Tower that can parry while being dual-wielded.
	say However, the downside is that they don't inflict a lot of damage.
	shrug
endif
~
|

