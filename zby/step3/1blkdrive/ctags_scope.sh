#!/bin/sh
find . -name "*.c" -o -name "*.h" >cscope.files
cscope -bkq -i cscope.files
ctags -R --languages=c --c-kinds=+lpx --fields=+aiKS --extra=+q
