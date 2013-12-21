>give_prog 100~
if charobj(5377)
	pmote examines $O closely.
	identify $o
	give $o $n
	chown "note stw" $n
	mpjunk "note stw"
	pmote rips up a note to the wizard.
endif
~
>greet_prog 100~
if charobj(5377)
	peer $n
	say I see you have a note from Penna...  I'll tell you what I'll do...
	say Give me an item, and I'll identify it for you.
endif
~
>rand_prog 5~
if rand (30)
	ponder
else
	meditate
endif
~
|
