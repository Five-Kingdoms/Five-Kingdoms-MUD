>greet_prog 100~
if rand (10)
	nod $n
	say Bail for the accused is set at 50,000 gold pieces.
	smirk
	say Pay it, and they're free.
endif
if rand (33)
	say Freedom comes at a price.
	snicker
	say My price is 50,000 gold pieces.
endif
~
>justice_prog 100~
say Welcome $N, Upholder of the Laws.
say We've got the dogs locked up for ya. Be careful back there now.
grin
emote hands you they keys to the Dungeon.
load obj 20850
give key $n
nod $n
~                   
>rand_prog 5~
if rand (50)
	pmote scratches his fat belly, and messes with his keys.
else
	pmote belches rudely, and adjusts his halberd.
endif
~
>bribe_prog 50000~
mpjunk gold
say Alright, a deal is a deal.
load obj 20850
give key $n
say Last door at the end.
smirk $n
setquest $n prison1 1
~
>bribe_prog 1~
mpjunk gold
say Yeah, like THAT'S enough!
cackle
dismay $n
~
|

