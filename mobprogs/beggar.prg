>rand_prog 3~
if rand (50)
	say Care to spare any change?
else
	beg $r
endif
~
>bribe_prog 1000~
worship $n
yell $N is such a kind soul!
~
>bribe_prog 500~
dance
say Thank you!
~
>bribe_prog 100~
smile $n
say Thanks for caring.
~
>bribe_prog 2~
smile $n
~
>bribe_prog 1~
yell You cheapskate!
if isnghost ($n)
	murder $n
endif
~
|
