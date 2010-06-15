SUBDIRS=linux ppc lpceurobot # spejblarm h8eurobot h8mirosot

all := all $(filter-out all Makefile,$(MAKECMDGOALS))

.PHONY: $(all) $(SUBDIRS)

$(all): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
