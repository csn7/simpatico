# vim: set ts=2:

PREFIX      = ../srclib/_prefix
CXX         = g++
CXXFLAGS    = -Wall -O3
CPPFLAGS    = -I$(PREFIX)/include -I$(PREFIX)/include/FL/images
LINK.o      = $(CXX)
FLTK_CONFIG = $(PREFIX)/bin/fltk-config
FLUID       = $(PREFIX)/bin/fluid

LOADLIBES = \
	$(shell $(FLTK_CONFIG) --use-gl --use-images --ldstaticflags) \
	$(PREFIX)/lib/libboost_iostreams.a \
	$(PREFIX)/lib/libboost_program_options.a \
	$(PREFIX)/lib/libbz2_static.a

ifeq ($(MODE),test)
CPPFLAGS += -DBOOST_TEST_MAIN
LOADLIBES += $(PREFIX)/lib/libboost_unit_test_framework.a
SOURCE = $(wildcard test_*.cpp)
endif

ifeq ($(MODE),main)
SOURCE = pregrid_dailysst.cpp pregrid_msm.cpp viewer.cpp
endif

TARGET = $(SOURCE:.cpp=)

all::
	$(MAKE) _preprocess
	$(MAKE) dependency=check MODE=test _build
	$(MAKE) dependency=check MODE=main _build
	$(MAKE) _postprocess
	$(MAKE) test

test::
	$(MAKE) MODE=test _test

clean::
	$(MAKE) MODE=test _clean
	$(MAKE) MODE=main _clean
	$(RM) *.hxx *.cxx *.d *.dSYM *.o

_preprocess:: viewer_ui.cxx

_build: $(TARGET)

_postprocess:: viewer
	for i in $^; do $(FLTK_CONFIG) --post $i || exit 1; done

_test:: $(TARGET)
	for i in $^; do ./$$i || exit 1; done

_clean::
	$(RM) $(TARGET)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -M $< $(OUTPUT_OPTION)

%.hxx : %.fl
	$(FLUID) -c $<

%.cxx : %.fl
	$(FLUID) -c $<

ifneq ($(dependency),)
include $(SOURCE:.cpp=.d)
endif
