>entry_prog 10~
say Can you help me?  I'm lost.
~
>rand_prog 3~
if rand(33)
	pmote examines a map carefully.
else
	if rand(50)
		ponder
	else
		boggle
	endif
endif
~
|

