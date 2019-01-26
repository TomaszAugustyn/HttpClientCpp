SUBDIRS = src test
ODIR = build

subdirs:
	for dir in $(SUBDIRS); do \
	   $(MAKE) -C $$dir; \
	done
	
cleanAll:
	rm -f $(ODIR)/*