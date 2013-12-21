>fight_prog 25~
if rand(50)
     echo A squad of archers launches a volley of flaming arrows at you!
     cast 'flame arrow' $n
else
     echo A wizard emerges from the parapets and utters an incantation!
     cast 'hellstream' $n
endif
~
|
