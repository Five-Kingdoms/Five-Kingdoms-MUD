>greet_prog 5~
say Do not take anything, or face the consequences.
~
>rand_prog 3~
if rand(50)
	pmote watches $R carefully.
else
	grumble
endif
~
>get_prog 100~
if isnghost ($n)
setquest $n valley 1
yell Die $N, you fool!
murder $n
endif
~
|

