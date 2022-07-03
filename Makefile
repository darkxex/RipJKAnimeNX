#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# EXEFS_SRC is the optional input directory containing data copied into exefs, if anything this normally should only contain "main.npdm".
# ROMFS is the directory containing data to be added to RomFS, relative to the Makefile (Optional)
#
# NO_ICON: if set to anything, do not use icon.
# NO_NACP: if set to anything, no .nacp file is generated.
# APP_TITLE is the name of the app stored in the .nacp file (Optional)
# APP_AUTHOR is the author of the app stored in the .nacp file (Optional)
# APP_VERSION is the version of the app stored in the .nacp file (Optional)
# APP_TITLEID is the titleID of the app stored in the .nacp file (Optional)
# ICON is the filename of the icon (.jpg), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.jpg
#     - icon.jpg
#     - <libnx folder>/default_icon.jpg
#---------------------------------------------------------------------------------
VERSION_MAJOR := 2
VERSION_MINOR := 5
VERSION_MICRO := 7

APP_TITLE	:=	RipJKAnimeNX
APP_AUTHOR	:=	AngelXex & Kronos2308
APP_VERSION	:=	${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}

TARGET		:=	$(subst $e ,_,$(notdir $(APP_TITLE)))
OUTDIR		:=	out
BUILD		:=	build
SOURCES		:=	source
#source/NSP source/NSP/data source/NSP/extra source/NSP/install source/NSP/nx source/NSP/nx/ipc
DATA		:=	data
INCLUDES	:=	include source
EXEFS_SRC	:=	RipJKForwader/exefs_src
ROMFS		:=	romfs
ICON        :=  Icon.jpg
APP_TITLEID := 05B9DB505ABBE000
#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
export APP_VERSION
ARCH	:=	-march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS	:=	-g -O3 -ffunction-sections \
			-Wall -Wno-write-strings \
			$(ARCH) $(DEFINES) \
			-DVERSION_MAJOR=${VERSION_MAJOR} \
			-DVERSION_MINOR=${VERSION_MINOR} \
			-DVERSION_MICRO=${VERSION_MICRO} \
			`freetype-config --cflags` \
			`sdl2-config --cflags`

CFLAGS	+=	$(INCLUDE) -D__SWITCH__ -D_GNU_SOURCE=1 
#CFLAGS	+=	-DTITLE='"$(APP_TITLE)"' -DVERSION='"$(APP_VERSION)"'

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++17 -fexceptions

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-no-as-needed,-Map,$(notdir $*.map)

LIBS	:=	-lnsp -lSDL2_ttf -lSDL2_gfx -lSDL2_image -lpng -lwebp -ljpeg `sdl2-config --libs` `freetype-config --libs` -lcurl -lmbedtls -lmbedx509 -lmbedcrypto -lz -lnx  `$(PREFIX)pkg-config --libs sdl2 SDL2_mixer SDL2_image SDL2_ttf` \
#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(LIBNX)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

#Get SOURCE Subdirs
#SOURCES	:=	$(foreach dir,$(SOURCES),$(shell find $(dir) -maxdepth 10 -type d))
#$(error $(SOURCES))

export OUTPUT	:=	$(CURDIR)/$(OUTDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export BUILD_EXEFS_SRC := $(TOPDIR)/$(EXEFS_SRC)

ifeq ($(OS),Windows_NT)
	FILENAME := hacbrewpack.exe
else
	FILENAME := hacbrewpack
endif

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(CURDIR)/$(OUTDIR)/$(TARGET).nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif

LOWER_TITLEID  = $(shell echo $(APP_TITLEID) | tr A-Z a-z)

.PHONY: $(BUILD) clean all NSP

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@echo '{"V":"$(APP_VERSION)"}'>romfs/V
	@[ -d $@ ] || mkdir -p $@
	@[ -d $(CURDIR)/$(OUTDIR) ] || mkdir -p $(CURDIR)/$(OUTDIR)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	
#---------------------------------------------------------------------------------
NSP: $(BUILD)
	@[ -d $(CURDIR)/$(OUTDIR)/control ] || mkdir -p $(CURDIR)/$(OUTDIR)/control
	@cp $(OUTPUT).nacp $(CURDIR)/$(OUTDIR)/control/control.nacp
	@cp $(CURDIR)/Icon.jpg $(CURDIR)/$(OUTDIR)/control/icon_AmericanEnglish.dat
	@$(CURDIR)/RipJKForwader/BuildTools/$(FILENAME) -k $(CURDIR)/RipJKForwader/BuildTools/keys.dat --titleid $(APP_TITLEID) --exefsdir $(BUILD)/exefs --romfsdir $(CURDIR)/romfs  --logodir $(CURDIR)/RipJKForwader/Logo --controldir $(CURDIR)/$(OUTDIR)/control --htmldocdir $(CURDIR)/RipJKForwader/HtmlDoc --backupdir $(CURDIR)/$(OUTDIR)/backup --nspdir $(CURDIR)/$(OUTDIR) --keyareakey 436875636B4E6F727269734973474F44
	@rm -rf $(CURDIR)/$(OUTDIR)/control
	@rm -rf $(CURDIR)/$(OUTDIR)/backup
	@mv $(OUTDIR)/$(LOWER_TITLEID).nsp '$(OUTDIR)/$(APP_TITLE)[$(APP_TITLEID)][v0].nsp'

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).nso $(TARGET).elf


#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all	: $(OUTPUT).nso $(OUTPUT).pfs0 $(OUTPUT).nro $(OUTPUT).nacp

$(OUTPUT).nso	:	$(OUTPUT).elf

ifeq ($(strip $(NO_NACP)),)
$(OUTPUT).nro	:	$(OUTPUT).elf $(OUTPUT).nacp
else
$(OUTPUT).nro	:	$(OUTPUT).elf
endif

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------