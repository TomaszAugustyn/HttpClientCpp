SUBDIRS = src test

subdirs:
	for dir in $(SUBDIRS); do \
	   $(MAKE) -C $$dir; \
	done
