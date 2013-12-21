>rand_prog 3~
if rand(33)
	emote throws up after drinking too much.
else
	if rand(40)
		emote takes another drink from the bottle in his hand.
	else
		say Can you spare some change for me to buy more drinks?
	endif
endif
~
>bribe_prog 1000~
dance
say Sir, you are too kind! Now I can buy more drinks!
emote buys a bottle of local speciality.
emote drinks local specialty from a bottle of local speciality.
say Ahh... my favorite!
~
>bribe_prog 750~
hop
say Thank you! Now I have enough gold for some rum!
emote buys a bottle of rum.
emote drinks rum from a bottle of rum.
say Rum... Mmm...
~
>bribe_prog 200~
bow $n
say Thank you for your help. I can now buy some salt water!
emote buys a bottle of salt water.
emote drinks salt water from a bottle of salt water.
emote smiles in satisfaction.
~
>bribe_prog 2~
mpjunk gold
smile $n
say Thank you sir for you help!
~
>bribe_prog 1~
mpjunk gold
yell $N, don't insult me with your single gold coin!
if isnghost ($n)
murder $n
endif
~
|
