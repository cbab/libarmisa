DOC=html latex
PREFIX=/usr/local

DOC_INSTALL_DIR=$(PREFIX)/share/doc/libarmisa

.PHONY: all install uninstall doxygen clean

all: doxygen

install:
	mkdir -p $(DOC_INSTALL_DIR)
	cp -a $(DOC) $(DOC_INSTALL_DIR)

uninstall:
	rm -rf $(DOC_INSTALL_DIR)

doxygen:
	doxygen Doxyfile

clean:
	rm -rf $(DOC)
