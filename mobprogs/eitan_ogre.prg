>fight_prog 100~
if rand(10)
    emote looks you over with his angry gaze.
    say You dare attack the Ogre Chieftain?!
    yell Guards! $n is disturbing the peace!
    force ogre rescue crunch
    force ogre murder $n
    say Do not let $n leave here alive!
    emote lets out a bestial roar!
else
    growl $n
    force ogre rescue crunch
    force ogre murder $n
endif
~

>rand_prog 30~
if rand(25)
    emote shifts his weight on the throne, and looks over some scrolls.
else
    emote sighs, and rubs his tired face.
endif
~
|
