>fight_prog 50~
if rand(50)
    emote looks you over with his angry gaze.
    say Violence is not tolerated in Shasarazade!
    yell Guards! $n is disturbing the peace!
    force duke rescue duchess
    force duke murder $n
else
    yell Help! I am being attacked by $n!
    force duke rescue duchess
    force duke murder $n
endif
~

>rand_prog 30~
if rand(25)
    emote smiles, and beams a smile politely at you.
else
    emote smiles wistfully at her loving husband.
endif
~
|
