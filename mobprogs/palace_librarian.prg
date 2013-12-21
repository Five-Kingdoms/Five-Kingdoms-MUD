>greet_prog 100~
if charobj (4744)
	say Tisk, tisk $n! Reference books can't be checked out!
	chown "book" $n
	say Now I have to refile this...
	mutter
	west
	emote files a book back into the shelves.
	drop book
	east
endif
if charobj (4749)
        say Tisk, tisk $n! Reference books can't be checked out!
        chown "book" $n
        say Now I have to refile this...
        mutter
        west
	emote files a book back into the shelves.
        drop book
        east
endif 
if charobj (4710)
        say Tisk, tisk $n! Reference books can't be checked out!
        chown "book" $n
        say Now I have to refile this...
        mutter
        west
	emote files a book back into the shelves.
        drop book
        east
endif
if charobj (4711)
        say Tisk, tisk $n! Reference books can't be checked out!
        chown "book" $n
        say Now I have to refile this...
        mutter
        west
        emote files a book back into the shelves.
        drop book
        east
endif
if rand (10)
	say Greetings, $n. Enjoy your stay here at the Val Miran Library.
endif
if rand (33)
	say Learning is fundamental, $n! Enjoy your time here!
endif
~
>rand_prog 5~
if rand (50)
	pmote shuffles though some research papers.
else
	pmote jots some notes down on a pad of paper.
endif
~
|

