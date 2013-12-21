>fight_prog 10~
yell Guards!  Guards!  $N is attacking me!
~
>greet_prog 5~
if israce($n) == 7
	say We could use someone like you in our village, $N.
else
	say Welcome to my village.  Don't cause any trouble, or else.
endif
~
>rand_prog 3~
if rand (50)
	ponder
else
	say My scientists are known to be among the wisest.
endif
~
|

