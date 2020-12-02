server_dir := SMTPServer
client_dir := SMTPClient

exe_directory := exe_directory="../../"
compile_flags := compile_flags="-g -g3 -O0 -D_GNU_SOURCE -DDEBUG"

ifeq ($(build),release)
	compile_flags := compile_flags="-O3 -D_GNU_SOURCE"
endif

all: server client

.PHONY: server
server: server_build_dir 
	cd $(server_dir) && make $(exe_directory) $(compile_flags) --directory=build --makefile=../Makefile 

server_build_dir:
	mkdir -p $(server_dir)/build

# Коля сюда добавь свое
.PHONY: client
client: client_build_dir 
	echo client

client_build_dir:
	mkdir -p $(client_dir)/build

.PHONY: clean
clean: server_clean client_clean

server_clean:
	cd $(server_dir) && make clean

# Коля сюда добавь свое
client_clean:
	echo client_clean 