>rand_prog 2~
if rand(25)
	say Does anyone have some spare gold for a poor old man?
else
	if rand(33)
		say Can you spare anything for this poor old man?
	else
		if rand(50)
			pmote hunches over holding his stomach in hunger.
		else
			pmote reaches $l hand toward you hoping for some donation.
		endif
	endif
endif
~
>bribe_prog 1000~
mpjunk gold
worship $n
yell $N is such a kind soul!
~
>bribe_prog 500~
mpjunk gold
dance
say Thank you kind sir!
~
>bribe_prog 100~
mpjunk gold
bow $n
say Thank you for helping one as poor as I.
~
>bribe_prog 2~
mpjunk gold
smile $n
~
>bribe_prog 1~
mpjunk gold
yell $N is a cheapskate!
if isnghost ($n)
murder $n
endif
~
|
