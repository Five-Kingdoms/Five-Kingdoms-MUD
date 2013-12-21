>greet_prog 100~
if charobj(9585)
	say Oh, such joy, $N, you have a pen!
	chown "pen" $n
	mpjunk pen
	say Now, $N, I will write a note to my lost love!
	say Please see that she gets it!
	pmote scribes a hasty letter on a sheet of parchment.
	load obj 4492
	give letter $n
	say Go with God, $N. Thank you!
	c 'bless' $n
	pmote smiles, his face tired and aging.
endif
if rand (33)
	say Greetings, $N. You look like you've traveled far.
	say Come rest with me, and I will tell you a story.
endif
~
>rand_prog 5~
if rand (50)
	pmote sits down, and takes a shallow breath.
	say My love left this city when the Blight came.
	say I hear she was headed toward the city of Revelsport.
	sigh
	say Though I've not spoken to her in years.
	say If I only had a pen, I would write her!
else
	pmote rests, and pines for his lost love.
endif
~
|

