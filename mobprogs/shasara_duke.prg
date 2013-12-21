>fight_prog 10~
if rand(50)
    emote looks you over with his angry gaze.
    say Violence is not tolerated in Shasarazade!
    yell Guards! $n is disturbing the peace!
    force duchess rescue duke
    force duchess murder $n
else
    yell Help! I am being attacked by $n!
    force duchess rescue duke
    force duchess murder $n
endif
~

>rand_prog 30~
if rand(25)
    emote smiles, and nods politely at you.
else
    emote sighs, and rubs his tired face.
endif
~
|
