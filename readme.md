minimum demonstration implementation of grisu, < 100 lines of code
focus on code clarity and simplicity
does not bother with the cache of powers of ten, just use pow()
this is grisu v1 which makes no attempt to format the output
so this is only suitable for serializing doubles for network transfer

build: gcc grisu.c -o grisu -lm
run: ./grisu

not sure if posix guarantees identity property is preserved for
doubles passed on cmd line, but c standard does, so for now the
values are entered in the source
