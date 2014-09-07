DEBUG=debug
RELEASE=release

.PHONY: all
all: dbg | $(DEBUG)

$(DEBUG):
	mkdir -p $(DEBUG)

$(RELEASE):
	mkdir -p $(RELEASE)

.PHONY: clean
clean:
	$(RM) -r $(DEBUG)
	$(RM) -r $(RELEASE)

.PHONY: release
rel: | $(RELEASE)
	cd $(RELEASE); cmake -DCMAKE_BUILD_TYPE=Release ..; $(MAKE)

.PHONY: debug
dbg: | $(DEBUG)
	cd $(DEBUG); cmake -DCMAKE_BUILD_TYPE=Debug ..; $(MAKE)

.PHONY: multi
multi:
	$(MAKE) clean $(RELEASE) $(DEBUG)
	$(MAKE) all -j 4

.PHONY: 2d
2d: dbg
	$(DEBUG)/powerdiagram ./examples/pd_bsp_2dCells_small_sites.csv ./examples/pd_bsp_2dCells_small_gamma.csv

.PHONY: simple
simple: dbg
	$(DEBUG)/powerdiagram ./examples/simple_sites.csv ./examples/simple_gamma.csv

.PHONY: run
run: 2d
