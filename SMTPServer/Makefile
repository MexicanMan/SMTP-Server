program_name := $(addprefix $(exe_directory),server) 
source_dirs  := . ../SMTPShared ../SMTPShared/error ../SMTPShared/logger
version := -std=c99

source_dirs      := $(addprefix ../,$(source_dirs))
search_wildcards := $(addsuffix /*.c,$(source_dirs))

$(program_name): $(notdir $(patsubst %.c,%.o, $(wildcard $(search_wildcards) ) ) )
	gcc $(version) $^ -o $@ 

VPATH := $(source_dirs) 

%.o: %.c
	gcc $(version) -c -MD $(compile_flags) $(addprefix -I,$(source_dirs)) $<

include $(wildcard *.d) 

.PHONY: test
test:
	echo test

.PHONY: clean
clean:
	rm -rf build