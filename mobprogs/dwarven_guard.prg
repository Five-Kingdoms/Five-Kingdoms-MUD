>rand_prog 3~
if rand (50)
	pmote watches $R closely.
else
	pmote paces back and forth.
endif
~
>greet_prog 5~
if israce($n) == 4
	bow $n
else
	pmote looks at $N suspiciously.
endif
~
|

