#! /usr/bin/env python
import sys
sys.path.append('/home/donghq/git/scripts')
from mylib  import run

run("gcc -D_DALLOC_ -g -o core1_dym core1.c");
run("gcc -o core1 -g core1.c");
