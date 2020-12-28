SOURCE_DIR := ../SMTPClient
AUTOGEN_DIR := $(SOURCE_DIR)/autogen

REPORT_NAME := report_client

TEX_DIR := ./tex
TEXINC_DIR := $(TEX_DIR)/include
UTILS_DIR := ./utils
DOT_DIR := ./dot
DOX_DIR := ./doxygen

FSM2DOT := $(UTILS_DIR)/fsm2dot
CFLOW2DOT := $(UTILS_DIR)/cflow2dot
MAKESIMPLE := $(UTILS_DIR)/makesimple
SRC2TEX := $(UTILS_DIR)/src2tex

CFLOW := cflow --level "0= "
SIMPLE_CFLOW := grep -v -f cflow.ignore

# Файлы latex
TEXS := $(wildcard $(TEX_DIR)/*.tex)

$(REPORT_NAME): $(TEXS) doxygen $(addprefix $(TEXINC_DIR)/, client_def_dot.pdf cflow_main_dot.pdf cflow_handlers_dot.pdf client-opts.def.tex)
	cd $(TEX_DIR) && pdflatex $(REPORT_NAME).tex && pdflatex $(REPORT_NAME).tex && cp $(REPORT_NAME).pdf ..

# .def -> _def.dot
$(DOT_DIR)/%_def.dot: $(AUTOGEN_DIR)/%.def
	$(FSM2DOT) $< > $@

# src -> cflow.dot
$(DOT_DIR)/cflow_main.dot: $(addprefix $(SOURCE_DIR)/, main.c client.c)
	$(CFLOW) $^ | $(SIMPLE_CFLOW) | $(CFLOW2DOT) > $@

$(DOT_DIR)/cflow_handlers.dot: $(addprefix $(SOURCE_DIR)/, autogen/client_fsm_handlers.c autogen/client-fsm.c)
	$(CFLOW) $^ | $(SIMPLE_CFLOW) | $(CFLOW2DOT) > $@

# .dot -> _dot.tex
$(TEXINC_DIR)/%_dot.tex: $(DOT_DIR)/%.dot
	dot2tex -ftikz --autosize --crop  $< > $@

# _dot.tex -> _dot.pdf
$(TEXINC_DIR)/%_dot.pdf: $(TEXINC_DIR)/%_dot.tex
	pdflatex -output-directory $(TEXINC_DIR) $<

# client-opts.def -> .tex
$(TEXINC_DIR)/client-opts.def.tex: $(AUTOGEN_DIR)/client-opts.def
	$(SRC2TEX) $< 7 | grep -v descrip > $@

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