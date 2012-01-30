NAME=devguide
LATEX=pdflatex
BIBTEX=bibtex

.PHONY: all clean

all: $(NAME).pdf

$(NAME).pdf: $(NAME).tex refs.bib
	$(LATEX)  $(NAME).tex
	$(BIBTEX) $(NAME)
	$(LATEX)  $(NAME).tex
	$(LATEX)  $(NAME).tex

clean:
	rm -f \
	$(NAME).aux $(NAME).log $(NAME).lof \
	$(NAME).lot $(NAME).toc $(NAME).dvi \
	$(NAME).bbl $(NAME).blg $(NAME).pdf
