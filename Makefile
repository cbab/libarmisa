SUBDIRS=armv7 test doc

.PHONY: all install uninstall clean $(SUBDIRS)

all: $(SUBDIRS)

armv7:
	$(MAKE) -C $@

doc:
	$(MAKE) -C $@

test:
	$(MAKE) -C $@

install:
	for dir in $(SUBDIRS); do \
	  $(MAKE) -C $$dir install; \
	done

uninstall:
	for dir in $(SUBDIRS); do \
	  $(MAKE) -C $$dir uninstall; \
	done

clean:
	for dir in $(SUBDIRS); do \
	  $(MAKE) -C $$dir clean; \
	done
