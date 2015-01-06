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

.PHONY: 2dsmall
2dsmall: dbg
	$(DEBUG)/powerdiagram ./examples/pd_bsp_2dCells_small_sites.csv ./examples/pd_bsp_2dCells_small_gamma.csv

.PHONY: 2d
2d: dbg
	$(DEBUG)/powerdiagram ./examples/pd_bsp_2dCells_sites.csv ./examples/pd_bsp_2dCells_gamma.csv

.PHONY: wine
wine: dbg
	$(DEBUG)/powerdiagram ./examples/pd_bsp_wine_sites.csv ./examples/pd_bsp_wine_gamma.csv

.PHONY: four
four: dbg
	$(DEBUG)/powerdiagram ./examples/four_sites.csv ./examples/four_gamma.csv

.PHONY: voronoi
voronoi: dbg
	$(DEBUG)/powerdiagram ./examples/voronoi_sites.csv ./examples/voronoi_gamma.csv

.PHONY: garage
garage: dbg
	$(DEBUG)/powerdiagram ./examples/garage_sites.csv ./examples/garage_gamma.csv

.PHONY: run
run: 2d
