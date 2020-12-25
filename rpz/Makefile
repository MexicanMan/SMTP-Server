SOURCE_DIR := ../SMTPServer
AUTOGEN_DIR := $(SOURCE_DIR)/autogen
REGEXP_FILE := $(SOURCE_DIR)/commands_parser.h
SYSTEST_FILE := $(SOURCE_DIR)/tests/sys_test.py

REPORT_NAME := report_server

TEX_DIR := ./tex
TEXINC_DIR := $(TEX_DIR)/include
UTILS_DIR := ./utils
DOT_DIR := ./dot
DOX_DIR := ./doxygen

FSM2DOT := $(UTILS_DIR)/fsm2dot
RE2TEX := $(UTILS_DIR)/re2tex
CFLOW2DOT := $(UTILS_DIR)/cflow2dot
MAKESIMPLE := $(UTILS_DIR)/makesimple
SRC2TEX := $(UTILS_DIR)/src2tex

CFLOW := cflow --level "0= "
SIMPLE_CFLOW := grep -v -f cflow.ignore

# Файлы latex
TEXS := $(wildcard $(TEX_DIR)/*.tex)

$(REPORT_NAME): $(TEXS) doxygen regexp.tex $(addprefix $(TEXINC_DIR)/, server_def_dot.pdf cflow_main_dot.pdf cflow_handlers_dot.pdf server-opts.def.tex sys_test_py.tex)
	cd $(TEX_DIR) && pdflatex $(REPORT_NAME).tex && pdflatex $(REPORT_NAME).tex && cp $(REPORT_NAME).pdf ..

# .def -> _def.dot
$(DOT_DIR)/%_def.dot: $(AUTOGEN_DIR)/%.def
	$(FSM2DOT) $< > $@

# src -> cflow.dot
$(DOT_DIR)/cflow_main.dot: $(addprefix $(SOURCE_DIR)/, main.c server.c)
	$(CFLOW) $^ | $(SIMPLE_CFLOW) | $(CFLOW2DOT) > $@

$(DOT_DIR)/cflow_handlers.dot: $(addprefix $(SOURCE_DIR)/, commands_parser.c fsm_handlers.c autogen/server-fsm.c)
	$(CFLOW) $^ | $(SIMPLE_CFLOW) | $(CFLOW2DOT) > $@

# .dot -> _dot.tex
$(TEXINC_DIR)/%_dot.tex: $(DOT_DIR)/%.dot
	dot2tex -ftikz --autosize --crop  $< > $@

# _dot.tex -> _dot.pdf
$(TEXINC_DIR)/%_dot.pdf: $(TEXINC_DIR)/%_dot.tex
	pdflatex -output-directory $(TEXINC_DIR) $<

# regexps -> *.tex
.PHONY: regexp.text
regexp.tex: $(REGEXP_FILE)
	$(RE2TEX) $(REGEXP_FILE) $(TEXINC_DIR)

# server-opts.def -> .tex
$(TEXINC_DIR)/server-opts.def.tex: $(AUTOGEN_DIR)/server-opts.def
	$(SRC2TEX) $< 8 | grep -v descrip > $@

# sys_test.py -> *.tex
$(TEXINC_DIR)/sys_test_py.tex: $(SYSTEST_FILE)
	$(SRC2TEX) $< 3 > $@

.PHONY: doxygen
doxygen: doxygen.cfg 
	doxygen doxygen.cfg
	cp $(DOX_DIR)/latex/*.tex $(TEXINC_DIR)
	cp $(DOX_DIR)/latex/*.sty $(TEX_DIR)

.PHONY: clean
clean:
	rm -rf $(TEXINC_DIR)/* $(DOT_DIR)/* $(DOX_DIR)/*; \
	rm $(TEX_DIR)/$(REPORT_NAME).pdf
	find $(TEX_DIR)/ -type f ! -name "*.tex" -and ! -name "*.pdf" -exec rm -f {} \; \