;; This buffer is for notes you don't want to save, and for Lisp evaluation.
;; If you want to create a file, visit that file with C-x C-f,
;; then enter the text in that file's own buffer.

cp $1 $1.old
cat $1 | sed s/Val Miran/Revelsport/g > $1;
