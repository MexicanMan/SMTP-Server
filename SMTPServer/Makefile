program_name := server
full_program_name := $(addprefix $(exe_directory),$(program_name)) 
source_dirs  := . autogen ../SMTPShared ../SMTPShared/error ../SMTPShared/logger
version := -std=c99

cflags:= -Wall -pedantic
ldflags := `autoopts-config ldflags`

source_dirs      := $(addprefix ../,$(source_dirs))
search_wildcards := $(addsuffix /*.c,$(source_dirs))

$(full_program_name): $(notdir $(patsubst %.c,%.o, $(wildcard $(search_wildcards))))
	gcc $(version) $^ -o $@ $(ldflags)

VPATH := $(source_dirs) 

%.o: %.c
	gcc $(version) -c -MD $(compile_flags) $(cflags) $(addprefix -I,$(source_dirs)) $<

include $(wildcard *.d) 

.PHONY: test
test:
	echo test

.PHONY: clean
clean:
	rm -rf build