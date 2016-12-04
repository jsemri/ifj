NAME=dokumentace

$(NAME).pdf: $(NAME).ps
	ps2pdf $(NAME).ps

$(NAME).ps: $(NAME).dvi
	dvips -t a4 $(NAME).dvi

$(NAME).dvi: $(NAME).tex
	latex $(NAME)
	bibtex $(NAME)
	latex $(NAME)
	latex $(NAME)

.PHONY:
	clean pack

clean:
	rm -f $(NAME).dvi $(NAME).aux $(NAME).toc $(NAME).log $(NAME).out $(NAME).ps $(NAME).bbl $(NAME).blg $(NAME).pdf
