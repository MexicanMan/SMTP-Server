program_name := server
full_program_name := $(addprefix $(exe_directory),$(program_name)) 
source_dirs := . autogen ../SMTPShared ../SMTPShared/error ../SMTPShared/logger tests
version := -std=c99

cflags:= -Wall -pedantic -Werror
ldflags := `autoopts-config ldflags` -lpcre -lm
test_ldflags := -lcunit

source_dirs := $(addprefix ../,$(source_dirs))
search_wildcards := $(addsuffix /*.c,$(source_dirs))
wildcard_source := $(wildcard $(filter-out ../tests/*.c,$(search_wildcards)))
wildcard_test := $(filter-out .././main.c,$(wildcard $(search_wildcards)))

$(full_program_name): $(notdir $(patsubst %.c,%.o,$(wildcard_source)))
	gcc $(version) $^ -o $@ $(ldflags)

.PHONY: test-run
test-run: test
	./test

.PHONY: test
test: $(notdir $(patsubst %.c,%.o,$(wildcard_test)))
	gcc $(version) $^ -o $@ $(ldflags) $(test_ldflags)

VPATH := $(source_dirs) 

%.o: %.c
	gcc $(version) -c -MD $(compile_flags) $(cflags) $(addprefix -I,$(source_dirs)) $<

include $(wildcard *.d) 

.PHONY: clean
clean:
	rm -rf build