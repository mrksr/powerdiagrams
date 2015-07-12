filter-false = $(strip $(filter-out 0 off OFF false FALSE,$1))
filter-true = $(strip $(filter-out 1 on ON true TRUE,$1))

BUILD_FOLDER?=build
RUN_FLAGS?=
INPUT_NAME?=voronoi
USE_BUNDLED_DEPS?=1

CMAKE_BUILD_TYPE?=Debug
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)
CMAKE_DEPS_FLAGS?=

ifneq (,$(USE_BUNDLED_DEPS))
  BUNDLED_CMAKE_FLAG := -DUSE_BUNDLED=$(USE_BUNDLED_DEPS)
endif

all: powerdiagram | $(DEBUG)

clean:
	$(RM) -r $(BUILD_FOLDER)

clean-deps:
	$(RM) -r .deps

distclean: | clean clean-deps

powerdiagram: $(BUILD_FOLDER)/.ran-cmake deps
	+$(MAKE) -C $(BUILD_FOLDER)

$(BUILD_FOLDER)/.ran-cmake: | deps
	cd $(BUILD_FOLDER) && cmake $(CMAKE_FLAGS) ..
	touch $@

deps: | build/.ran-third-party-cmake
ifeq ($(call filter-true,$(USE_BUNDLED_DEPS)),)
	+$(MAKE) -C .deps
endif

$(BUILD_FOLDER)/.ran-third-party-cmake:
ifeq ($(call filter-true,$(USE_BUNDLED_DEPS)),)
	mkdir -p .deps
	cd .deps && \
		cmake $(CMAKE_FLAGS) $(BUNDLED_CMAKE_FLAG) $(CMAKE_DEPS_FLAGS) ../third-party
endif
	mkdir -p $(BUILD_FOLDER)
	touch $@

run: powerdiagram
	$(BUILD_FOLDER)/powerdiagram $(RUN_FLAGS) ./examples/$(INPUT_NAME)_sites.csv ./examples/$(INPUT_NAME)_gamma.csv

2dsmall:
	$(MAKE) INPUT_NAME=pd_bsp_2dCells_small run

3dsmall:
	$(MAKE) INPUT_NAME=pd_bsp_3dCells_small run

2d:
	$(MAKE) INPUT_NAME=pd_bsp_2dCells run

3d:
	$(MAKE) INPUT_NAME=pd_bsp_3dCells run

wine:
	$(MAKE) INPUT_NAME=pd_bsp_wine run

four:
	$(MAKE) INPUT_NAME=four run

grid:
	$(MAKE) INPUT_NAME=grid run

voronoi:
	$(MAKE) INPUT_NAME=voronoi run

garage:
	$(MAKE) INPUT_NAME=garage run
