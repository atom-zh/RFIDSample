##
## Makefile for rfidsample4 executable for linux64
##


include common.mk


ifneq ($(MAKECMDGOALS),linux64_clean)
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif



### Edit this section for a new project : start #####
PRJNAME					:=rfidsample
BUILDDIR				:=
OSBUILDDIR				:=
### Edit this section for a new project : end #####

PLATFORMOUTDIR			:=$(BUILDDIR)$(GLOBALOUTDIR)
PROJOUTDIR				:=$(PLATFORMOUTDIR)/$(CONFIG)
OSPROJOUTDIR			:=$(OSBUILDDIR)$(GLOBALOUTDIR)$(sep)$(CONFIG)
OUTDIR					:=$(PLATFORMOUTDIR)/$(CONFIG)
SRCDIR					:=src
##INCPLATFORMDIR				:=../../../../Platform
INCROOTDIR				:=./
OUTPUTFILENAME			:=$(PRJNAME).elf
OUTPUTFILE				:=$(OUTDIR)/$(OUTPUTFILENAME)
PROJECTFLAGS			:=-DUNICODE -Dlinux
LIBDIR					:=-L$(OUTDIR) \
						-L$(INCROOTDIR)/lib \

LIBS					:=-lrfidapi32 -lpthread  -lz -lxml2 -lrt -lltk -lssh2

CXXFLAGS				:=$(GLOBALCXXFLAGS)

INCDIR					:= 	-I. \
						-I$(INCROOTDIR)/include \
						-I$(SRCDIR)/ce
					



SRC_REL_PATH1 :=	 

CXXFLAGS := $(patsubst -O3,, $(CXXFLAGS))

CPP_DEPS1 := $(wildcard $(PROJOUTDIR)/$(SRC_REL_PATH1)/*.d)
CPP_FILES1 := $(wildcard $(SRCDIR)/$(SRC_REL_PATH1)/*.cpp)
OBJ_FILES1 := $(patsubst $(SRCDIR)/$(SRC_REL_PATH1)/%.cpp,$(PROJOUTDIR)/$(SRC_REL_PATH1)/%.o,$(CPP_FILES1))
$(PROJOUTDIR)/$(SRC_REL_PATH1)/%.o : $(SRCDIR)/$(SRC_REL_PATH1)/%.cpp 
	$(CXX) $(PROJECTFLAGS) $(INCDIR) $(CXXFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" $(TARGET)  -o "$@" "$<"
   
CPP_DEPS :=  $(CPP_DEPS1)

##
## Build Targets
##

.PHONY : prebuild 
prebuild:
ifneq ($(CONFIG),linux64_clean)
	@echo "creating build directories"
ifeq (,$(wildcard $(OSPROJOUTDIR)))
	@$(MKDIR) $(OSPROJOUTDIR)
endif

endif
	##$(MAKE) -C ../rfidapi32 $(CONFIG)


$(OUTPUTFILE): $(OBJ_FILES1)
	@echo "linking.."
	$(CXX) $(LIBDIR) $(TARGET) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"  -o $(OUTPUTFILE) $(OBJ_FILES1) $(LIBS) $(RPATHOPTIONS) 
	
.PHONY : linux64_clean 
linux64_clean:
linux64_clean: prebuild
	$(RMDIR) $(PLATFORMOUTDIR)$(sep)linux64_release$(sep)$(PRJNAME)
	$(RMDIR) $(PLATFORMOUTDIR)$(sep)linux64_debug$(sep)$(PRJNAME)
	$(RM) $(PLATFORMOUTDIR)$(sep)linux64_release$(sep)$(OUTPUTFILENAME)
	$(RM) $(PLATFORMOUTDIR)$(sep)linux64_release$(sep)*.o
	$(RM) $(PLATFORMOUTDIR)$(sep)linux64_release$(sep)*.d
	$(RM) $(PLATFORMOUTDIR)$(sep)linux64_debug$(sep)$(OUTPUTFILENAME)
	$(RM) $(PLATFORMOUTDIR)$(sep)linux64_debug$(sep)*.o
	$(RM) $(PLATFORMOUTDIR)$(sep)linux64_debug$(sep)*.d


.PHONY : linux64_debug
linux64_debug: prebuild
linux64_debug: $(OUTPUTFILE)


.PHONY : linux64_release
linux64_release: prebuild
linux64_release: $(OUTPUTFILE)


.PHONY : linux64_all
hercules_all: prebuild
hercules_all: $(OUTPUTFILE)

