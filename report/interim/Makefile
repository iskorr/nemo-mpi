FILE=interim_report
DUMP=$(FILE).dump
TEX=$(FILE).tex

all: $(TEX)
	pdflatex $(TEX) &&  \
	bibtex $(FILE) && \
    pdflatex $(TEX) && \
    pdflatex $(TEX) ;

clean:
	rm -f $(FILE).bbl $(FILE).aux $(FILE).blg $(FILE).dvi $(FILE).log $(FILE).toc $(FILE).pdf
