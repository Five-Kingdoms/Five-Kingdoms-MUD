>greet_prog 100~
if charobj (8920)
	say If you give me 500 gold, I could turn your gyvel herb into a powerful potion, $N.
endif
if rand (80)
	say If you could find me some herbs, I could make something powerful.
endif
~
>rand_prog 5~
if rand (50)
	say Gyvel leaves can cure temporary blindness.
endif
say Of course, these herbs must be processed properly.
~
>bribe_prog 500~
mpjunk gold
if charobj (8920)
  chown "gyvel herb" $n
  mpjunk gyvel
  load obj 8926
  give gyvel $n
  say There you go.  Use it if you've been blinded.
  setquest $n plains2 1
else
  say Thanks for the money.
  say You know, if you had the right herbs...
endif
~
>bribe_prog 1~
mpjunk gold
say Thanks for the money, but it's not enough for my help.
~
|

