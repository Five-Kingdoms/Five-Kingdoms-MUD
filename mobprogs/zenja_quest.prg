>greet_prog 100~
if charobj(4492)
	say Looks like you have a scroll there of some kind, $N.
	say Let's take a look at that.
	chown "letter" $n
	mpjunk letter
	gasp
	say Praise the Gods, Lorien is alive!
	say Oh, dear, I must go to him!
	ponder
	say But what to give you, dear $N?
	ponder
	say Aha! I have just the thing!
	say This is the Nectar of the Gods.
	say Use it when you're in a truly bad situation, $N.
	load obj 20801
	give nectar $n
	say Go with God, $N. Thank you!
	hug $n
	pmote smiles, and rushes to pack.
	setquest $n lorien 1
	up
endif
~
|

