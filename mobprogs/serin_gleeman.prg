>rand_prog 25~
if rand(33)
	say I know many songs from foreign lands.
else
	if rand(50)
		say Can I interest you in a song from a foreign land?
	else
		say I would gladly present a song to you for only 100 gold pieces.
	endif
endif
~
>bribe_prog 100~
mpjunk gold
say Here's a song for you, $N!
emote clears his throat.
if rand(20)
	emote sings '`6We rode down River Iralell`7'
	emote sings '`6just to see the Tairen come.`7'
	emote sings '`6We stook along the riverbank`7'
	emote sings '`6with the rising of the sun.`7'
	emote sings '`6Their horses blacked the summer plain,`7'
	emote sings '`6their banner blacked the sky.`7'
	emote sings '`6But we stood our ground on the banks of River Irallel.`7'
	emote sings '`6Oh we stood our ground.`7'
	emote sings '`6Yes we stood our ground.`7'
	emote sings '`6Stood our ground along the river in the morning.`7'
	smile
	bow
else
	if rand(25)
		emote sings '`6My love is gone, carried away`7'
		emote sings '`6by the wind that shakes the willow,`7'
		emote sings '`6and all the land is beaten hard`7'
		emote sings '`6by the wind that shakes the willow.`7'
		emote sings '`6But I will hold her close to me`7'
		emote sings '`6in heart and dearest memory,`7'
		emote sings '`6and with her strength to steel my soul,`7'
		emote sings '`6her love to warm my heart-strings,`7'
		emote sings '`6I will stand where we once sang,`7'
		emote sings '`6though cold wind shakes the willow.`7'
		smile
		bow
	else
		if rand(33)
			emote sings '`6My home is waiting there for me,`7'
			emote sings '`6and the girl I left behind.`7'
			emote sings '`6Of all the treasure that waits for me,`7'
			emote sings '`6that's what I want to find.`7'
			emote sings '`6Her eyes so merry, and her smile so sweet,`7'
			emote sings '`6her hugs so warm, and her ankle neat,`7'
			emote sings '`6her kisses hot, now there's a treat.`7'
			emote sings '`6If there's a treasure greater, it lies not in my mind.`7'
			smile
			bow
		else
			if rand(50)
				emote sings '`6I'll dance with a girl with eyes of brown,`7'
				emote sings '`6or a girl with eyes of green,`7'
				emote sings '`6I'll dance with a girl with any color eyes,`7'
				emote sings '`6but yours are the prettiest I've seen.`7'
				emote sings '`6I'll kiss a girl with hair of black,`7'
				emote sings '`6or a girl with hair of gold,`7'
				emote sings '`6I'll kiss a girl with any color hair,`7'
				emote sings '`6but it's you I want to hold.`7'
				smile
				bow
			else
				emote sings '`6A Lugard girl, she came to town, to see what she could see.`7'
				emote sings '`6With a wink of her eye, and a smile on her lip,`7'
				emote sings '`6she snagged a boy, or three, or three.`7'
				emote sings '`6With an ankle slim, and skin so pale,`7'
				emote sings '`6she caught the owner of a ship, a ship.`7'
				emote sings '`6With a soft little sigh, and a gay little laugh,`7'
				emote sings '`6she made her way so free. So free.`7'
				smile
				bow
			endif
		endif
	endif
endif
~
|
