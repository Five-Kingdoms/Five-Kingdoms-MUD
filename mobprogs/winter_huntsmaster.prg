>rand_prog 5~
if rand(50) 
  pmote scans around him.
else
  pmote checks the ground for tracks.
  say There is nothing that can best my tracking abilities.
endif
~


>give_prog 100~
if ispc($n)
if number ($o) == 10329
say You found my lucky arrow!  I am indebted to you.
say Please accept this animal skin from a displacer beast. It is worth much at any furs store.
load obj 10330 
give dark $n
endif
endif
~
|
