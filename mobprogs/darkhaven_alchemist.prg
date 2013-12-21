>rand_prog 3~
if isasleep($i)
  break
endif
if rand(50)
	say Beware the roads around Zalango, the dead travel fast...
else
	load obj 1622
	emote places the potion on the shelf.
	shiver
	say I hate to think of what travels the roads this night.. 
endif
~
|