rm cscop.files cscope.out
find . -name '*.cc' >> cscope.files
find . -name '*.h' >> cscope.files
find . -name '*.c' >> cscope.files
cscope -pkR

