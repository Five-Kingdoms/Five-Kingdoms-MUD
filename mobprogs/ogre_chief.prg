>rand_prog 3~
if rand (50)
	growl
else
	growl $r
endif
~
>greet_prog 25~
if rand (10)
	cackle
	say You must be brave to enter our village, $N.
else
	if isnghost ($n)
		say Come closer $N, I want to show you something.
		murder $N
	endif
endif
~
|

