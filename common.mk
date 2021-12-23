##
## Common settings for Thredbo
##

## Configuration: release/debug

ifeq ($(MAKECMDGOALS),all)
CONFIG					:=debug
endif
ifeq ($(MAKECMDGOALS),debug)
CONFIG					:=debug
endif
ifeq ($(CONFIG),debug)
CONFIG					:=debug
#CONFIGFLAGS				:=-g3 -gdwarf-2 -O0
CONFIGFLAGS				:=-fno-tree-vrp -fexpensive-optimizations -fomit-frame-pointer -frename-registers -g -D_THREDBO_DEBUG
PLATFORM				:=thredbo
endif
ifeq ($(MAKECMDGOALS),package)
CONFIG					:=release
CONFIGFLAGS				:=-O3 -fexpensive-optimizations -fno-omit-frame-pointer -frename-registers -mfloat-abi=hard
PLATFORM				:=thredbo
endif
ifeq ($(MAKECMDGOALS),release)
CONFIGFLAGS				:=-O3 -fexpensive-optimizations -fno-omit-frame-pointer -frename-registers -mfloat-abi=hard
#CONFIGFLAGS				:=-O3 -fno-tree-vrp -fexpensive-optimizations -fomit-frame-pointer -frename-registers 
PLATFORM				:=thredbo
CONFIG					:=release
endif
ifeq ($(MAKECMDGOALS),clean)
CONFIG					:=clean
PLATFORM				:=thredbo
endif
##this is for the linux host machine build - valid only for API
ifeq ($(MAKECMDGOALS),linux)
CONFIG					:=debug
CONFIGFLAGS				:=-g3 -gdwarf-2 -O0
PLATFORM				:=linux
endif
##this is for the hercules platform build - valid only for API
ifeq ($(MAKECMDGOALS),hercules_all)
CONFIG					:=hercules_debug
endif
ifeq ($(MAKECMDGOALS),hercules_debug)
CONFIG					:=hercules_debug
endif
ifeq ($(CONFIG),hercules_debug)
CONFIG					:=hercules_debug
#CONFIGFLAGS				:=-g3 -gdwarf-2 -O0
CONFIGFLAGS				:=-fno-tree-vrp -fexpensive-optimizations -fomit-frame-pointer -frename-registers -g -D_THREDBO_DEBUG
PLATFORM				:=hercules
endif
ifeq ($(MAKECMDGOALS),hercules_package)
CONFIG					:=hercules_release
CONFIGFLAGS				:=-O3 -fexpensive-optimizations -fomit-frame-pointer -frename-registers 
PLATFORM				:=hercules
endif
ifeq ($(MAKECMDGOALS),hercules_release)
#CONFIGFLAGS				:=-O3 -fexpensive-optimizations -fomit-frame-pointer -frename-registers 
CONFIGFLAGS				:=-O3 -fno-tree-vrp -fexpensive-optimizations -fomit-frame-pointer -frename-registers 
PLATFORM				:=hercules
CONFIG					:=hercules_release
endif
ifeq ($(MAKECMDGOALS),hercules_clean)
CONFIG					:=hercules_clean
PLATFORM				:=hercules
endif
##this is for the Ubuntu host machine build - valid only for API
ifeq ($(MAKECMDGOALS),ubuntu_release)
CONFIG					:=ubuntu_release
CONFIGFLAGS				:=-O3 -fexpensive-optimizations -fomit-frame-pointer -frename-registers
PLATFORM				:=ubuntu
endif
ifeq ($(MAKECMDGOALS),ubuntu_debug)
CONFIG					:=ubuntu_debug
CONFIGFLAGS				:=-fno-tree-vrp -fexpensive-optimizations -fomit-frame-pointer -frename-registers -g -D_THREDBO_DEBUG
PLATFORM				:=ubuntu
endif
ifeq ($(MAKECMDGOALS),ubuntu_clean)
CONFIG					:=ubuntu_clean
PLATFORM				:=ubuntu
endif

ifeq ($(MAKECMDGOALS),linux64_release)
CONFIG					:=linux64_release
CONFIGFLAGS				:=-O3 -fexpensive-optimizations -fomit-frame-pointer -frename-registers
PLATFORM				:=linux64
endif
ifeq ($(MAKECMDGOALS),linux64_debug)
CONFIG					:=linux64_debug
CONFIGFLAGS				:=-fno-tree-vrp -fexpensive-optimizations -fomit-frame-pointer -frename-registers -g -D_THREDBO_DEBUG
PLATFORM				:=linux64
endif
ifeq ($(MAKECMDGOALS),linux64_clean)
CONFIG					:=linux64_clean
PLATFORM				:=linux64
endif


ifeq ($(OS),Windows_NT)
sep					:=\\
MKDIR				:=mkdir
COPY				:=copy
COPYDIR				:=xcopy /s /y /E
RM					:=cs-rm -f
RMDIR				:=$(RM) -r -d 
COMMANDSEPARATOR	:= &
TAR					:=..\build-tools\\bsdtar\\bin\\bsdtar.exe
else
sep					:=/
MKDIR				:=mkdir -p
COPY				:=cp
COPYDIR				:=cp -r -v
RM					:=rm -f
RMDIR				:=$(RM) -r
COMMANDSEPARATOR	:= ;
TAR					:=tar
SH                  :=/bin/sh
CHANGEDIR			:=cd
endif

ifeq ($(PLATFORM),linux)
TARGET				:=
else ifeq ($(PLATFORM),hercules)
TARGET				:=
else
#TARGET				:=
TARGET				:=-mcpu=cortex-a8
endif
ifeq ($(PLATFORM),ubuntu)
CXX					:=g++
CC					:=gcc
SOCXX				:=g++ -shared
AR					:=ar
STRIP				:=strip
TARGET				:=-m32
endif
ifeq ($(PLATFORM),linux64)
CXX					:=g++
CC					:=gcc
SOCXX				:=g++ -shared
AR					:=ar
STRIP				:=strip
TARGET				:=-m64
else
CXX					:=arm-montavista-linux-gnueabi-g++ -mfloat-abi=hard
CC					:=arm-montavista-linux-gnueabi-gcc -mfloat-abi=hard
SOCXX				:=arm-montavista-linux-gnueabi-g++ -shared
AR					:=arm-montavista-linux-gnueabi-ar  
STRIP				:=arm-montavista-linux-gnueabi-strip
endif
GLOBALCXXFLAGS		:=$(CONFIGFLAGS) -Wall -c -fmessage-length=0
GLOBALOUTDIR		:=bin
REMOTELIBPATH		:=/platform/lib /usr/lib /apps/bin/libs
RPATHOPTIONS		:="-Wl,-rpath,$(REMOTELIBPATH)"
ROOTFS              :=workspace/evmthredbo/tmp/work/am3505_fx7500-montavista-linux-gnueabi/small-image/1.0-r0/rootfs
THIRD-PARTY-TOOLS   :=workspace/evmthredbo/tmp/work/cortexa8hf-neon-montavista-linux-gnueabi
