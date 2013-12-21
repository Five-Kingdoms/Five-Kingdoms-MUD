>all_greet_prog 100~ 
if ispc($n)
echo You hear a shattering sound nearby as an image turns into reality!
trans mistdeatha
force mistdeatha rescue mistdeath
endif
~

>rand_prog 4~
if rand(50)
mpasound A dull sound of a strong blowing against the wall is here.
else
mpasound Distant wailing sounds are nearby.
endif
~
>entry_prog 100~
say entry
~

>death_prog 100~
at 78 load mob 10811
~
|
