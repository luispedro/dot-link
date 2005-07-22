#Template para Makefiles para compilar ficheiros de C++
#Variáveis importantes:
#
# PROGRAM : O progama alvo.

SOURCES := $(wildcard *.cpp)
OBJECTS_NAMES := $(SOURCES:.cpp=.o)
OBJECTS := $(patsubst %,.objs/%,$(OBJECTS_NAMES))
DEP_NAMES := $(SOURCES:.cpp=.d)
DEPS :=$(patsubst %,.deps/%,$(DEP_NAMES))

all: $(PROGRAM)

.deps:
	mkdir .deps

.objs: ; mkdir .objs


#Below is a workaround for the failure of ccache to properly handle
#the -MMD option

.objs/%.o: %.cpp
	@objdir=`dirname $@` && [ -d $$objdir ] || mkdir -p $$objdir
	@depdir=`dirname .deps/$<` &&  [ -d $$depdir ] || mkdir -p $$depdir
	(depfile=.deps/$(<:.cpp=.d) && \
	$(CXX) $(CXXFLAGS) -c -o $@ $< -Wp,-MMD,$${depfile}T \
	&&  prefix=`(head $${depfile}T | grep -q .objs) || echo -n $$(dirname $@)/` \
	&&  (echo -n $$prefix ; cat $${depfile}T) >  $${depfile}TT \
	&& mv $${depfile}TT $${depfile} \
	&& rm $${depfile}T)

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LIBS)

clean:
	rm -f $(PROGRAM) $(OBJECTS)

distclean: clean
	rm -rf .deps .objs

depclean:
	rm -rf .deps

.PHONY : clean all distclean

sinclude $(DEPS)  i_dont_exist_but_i_shut_the_warning_up

