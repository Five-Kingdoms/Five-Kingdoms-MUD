>rand_prog 5~
if rand (50)
	pmote works on his paperwork.	
else
	pmote pours over his safety plans.
endif
~
>greet_prog 5~
if israce($n) == 6
	say Hullo, $N!
else
	say Welcome to the Shire.
endif
~
|

