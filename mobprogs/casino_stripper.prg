>greet_prog 100~
if rand (10)
	wink $n
endif
if rand (33)
	flirt $n
endif
if rand (66)
	pmote sighs wistfully, and tosses her hair.
endif
if rand (80)
	bkiss $n
endif
~

>act_prog p kisses you.~
say That's sweet, $n, but do it again, and I'll cut off your balls.
glare $n
~

>act_prog p pecks you on the cheek, how sweet!~
say Back off me, $n.
mutter
~

>act_prog p pokes you in the ribs.~
say If you want to 'poke', you're gonna have to pay, $n.
smirk self
~

>act_prog p winks suggestively at you.~
say Something in your eye, there, $n?
giggle $n
~

>act_prog p tenderly caresses your body.~
say Touch me again, and you'll pull back a nub, $n.
say I don't work for free here.
mutter
~

>act_prog p licks your jawline delicately.~
say Gross, $n! If I wanted germs, I'd kiss an illithid!
spit
~

>act_prog p smirks.~
say Find something amusing, $n?
~

>act_prog p looks at you.~
say I see you looking. I can show you more, you know...
say IF the price is right, anyway.
grin
~

>act_prog p falls down laughing.~
say I assume you're laughing at your own inadequacies, $n?
snicker
~

>speech_prog p Tell me about yourself.~
say Well, what exactly do you want to know, $n?
~

>speech_prog p Where are you from?~
say Originally, I'm from here, in Revelsport.
say But I used to live in Zalango.
shiver
~

>speech_prog p Why did you leave?~
say No real work in Zalango.
say And the place is a slum.
chuckle
~

>speech_prog p Why did you move?~
say No real work in Zalango.
say And the place is a slum.
chuckle
~   

>speech_prog p How old are you?~
say I'll be twenty six in a month.
~

>speech_prog p You're cute.~
smile $n
say That's sweet. You're not half bad yourself, $n.
~  

>speech_prog p Do you like working here?~
say Oh, well, it pays the bills.
shrug
~

>speech_prog p Can I take you home with me?~
say Heh. How much money do you have?
grin $n
~

>speech_prog p Will you marry me?~
eye $n
say I'm not the marrying type, $n.
~

>speech_prog p I want you.~
chuckle $n
say Rather forward aren't you?
say But for the right price, you can have me.
~

>speech_prog p Fuck me.~
slap $N
say I don't like your language.
~

>speech_prog p I want to fuck you.~
slap $N
say Back off, punk.
~

>speech_prog p You're sexy.~
smile $n
say That's sweet. You're not half bad yourself, $n.
~

>speech_prog p Nice to meet you.~
say Nice to meet you, $n.
shake $n
~

>act_prog p chuckles politely.~
smile
~

>act_prog p nods.~
chuckle
~

>fight_prog 100~
say Oh no you don't!
yell Help! $N is attacking me!
trans vinnie
flee
~

>rand_prog 25~
if rand (20)
	pmote smiles at you, and adjusts her garters.
endif
if rand (40)
	pmote runs her fingers through her hair, and looks at you lustily.
endif
if rand (60)
	pmote ties her long hair back in a braid.
endif
if rand (80)
	pmote stretches, and arches her lithe back.
endif
~

>bribe_prog 5000~
mpjunk gold
if rand (50)
	say My, aren't we the big spender, $n?
	giggle $n
	say I guess you want to see more, now, hm?
	rem halter
	pmote runs her hands over her naked breasts.
	say Oh, you like that?
	pmote pulls $N's head closer to her perky breasts.
	pmote slowly bends over, and slides her underwear off.
	say Oooooh...
	rem bikini
	say That's what you wanted, wasn't it, $n?
	pmote runs her hands between her legs.
	say Oh, $N....You make me so hot...
	pmote traces $N's lips with her fingers.
	giggle
	say I think I'd like to take you upstairs...
	say But it will cost you...
	bkiss $n
	wear halter
	wear bikini
else
	say Mmmmmm...I like someone with money.
	smile $n
	say Do you like me, $n?
	say I'll show you what you do to me...
	rem halter
	pmote slides her hands over her breasts.
	pmote pulls $N close, into her chest.
	say Oh, yes...
	pmote slides off her underwear.
	rem bikini
	pmote takes $N's hand, and slides it between her legs.
	say Do you see how much I want you?
	lick $n
	say For some more gold, I could take you upstairs...
	say And show you some real fun...
	wink $n
	wear halter
	wear bikini
	bkiss $n
endif
~

>bribe_prog 1000~
mpjunk gold
if rand(50)
        say Oh, feeling lucky tonight, are we, $n?
        pmote sits on $N's lap, and wiggles.
        say Oh, yes...You have something down there for me, don't you?
        grin $N
        say Can you feel how hot you make me?
        pmote takes $N's hands, and runs them over her breasts.
        say Oh...and I'm even hotter down...there.  
        pmote takes $N's hands, and slides them over her thighs.
        echo With a deft action, she undoes her halter top.
        rem halter
        drop halter
        pmote turns around to face $N.
        say Now now, no touching...
        giggle
        emote runs her hands over her breasts.
        say Aaah....Shall I go further?
        wink $n
        get halter
        wear halter
else
        say If you have the gold, I have the time, $N.
        wink $n        
        pmote walks over, and sits on $N's lap.
        pmote runs her hands down $N's chest.
        say Oh, you're so big and strong.
        say But I think I know what you want to see...
        pmote undoes her halter top.
        rem halter
        drop halter
        pmote runs her hands over her breasts.
	pmote moans in pleasure.
        say Is that what you like, $N?
        pmote moans in pleasure as she touches her nipples.
        say Oh, I could do so much more...
        grin $N     
        get halter
        wear halter
endif
~  

>bribe_prog 500~
mpjunk gold
if rand(50)
	pmote smiles, and tucks the gold in her purse.
	say Time for some fun...
	pmote walks toward $N, swaying her hips, and sits on your lap.
	kiss $N
	pmote slides her top down so you can peek in...
	say Oh yeah, do you like that, $n? 
	pmote runs her fingers down your neck.
	say I can show you more. How much are you willing to spend?
	wink $n
else
	pmote tucks the gold in her purse with a nod.
	say Ooh....
	pmote walks toward $N, seductively.
	say I know what you want...
	pmote spreads her legs, and sits on $N's lap.
	say You want me, don't you?
	emote breathes heavily in $n's ear.
	emote slides $n's hands over her chest.
endif
~

>bribe_prog 100~
mpjunk gold
if rand(50)
	say My uncle has a goat you may be able to get for that, $n.
	tease $n
else
	say You can't even sniff my panties for that, $n.
	smirk
endif
~

>bribe_prog 1~
mpjunk gold
say I won't do anything for less than five hundred, cheapskate!
yell $n is a cheapskate!
slap $n
~
|

