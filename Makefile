### Generated by Winemaker 0.8.4
###
### Invocation command line was
### /usr/bin/winemaker --console --lower-uppercase -imsacm32 -ishell32 --wine32 .


SRCDIR                = .
SUBDIRS               =
DLLS                  =
LIBS                  =
EXES                  = wav2at3.exe



### Common settings

CEXTRA                = -mno-cygwin \
			-m32
CXXEXTRA              = -m32
RCEXTRA               =
DEFINES               =
INCLUDE_PATH          = -I.
DLL_PATH              =
DLL_IMPORTS           = msacm32 \
			shell32
LIBRARY_PATH          =
LIBRARIES             =


### wav2at3.exe sources and settings

wav2at3_exe_MODULE    = wav2at3.exe
wav2at3_exe_C_SRCS    =
wav2at3_exe_CXX_SRCS  = choose.cpp \
			findat3driver.cpp \
			main.cpp \
			util.cpp
wav2at3_exe_RC_SRCS   =
wav2at3_exe_LDFLAGS   = -mconsole \
			-mno-cygwin \
			-m32
wav2at3_exe_ARFLAGS   =
wav2at3_exe_DLL_PATH  =
wav2at3_exe_DLLS      = odbc32 \
			ole32 \
			oleaut32 \
			winspool \
			odbccp32
wav2at3_exe_LIBRARY_PATH=
wav2at3_exe_LIBRARIES = uuid

wav2at3_exe_OBJS      = $(wav2at3_exe_C_SRCS:.c=.o) \
			$(wav2at3_exe_CXX_SRCS:.cpp=.o) \
			$(wav2at3_exe_RC_SRCS:.rc=.res)



### Global source lists

C_SRCS                = $(wav2at3_exe_C_SRCS)
CXX_SRCS              = $(wav2at3_exe_CXX_SRCS)
RC_SRCS               = $(wav2at3_exe_RC_SRCS)


### Tools

CC = winegcc
CXX = wineg++
RC = wrc
AR = ar


### Generic targets

all: $(SUBDIRS) $(DLLS:%=%.so) $(LIBS) $(EXES)

### Build rules

.PHONY: all clean dummy

$(SUBDIRS): dummy
	@cd $@ && $(MAKE)

# Implicit rules

.SUFFIXES: .cpp .cxx .rc .res
DEFINCL = $(INCLUDE_PATH) $(DEFINES) $(OPTIONS)

.c.o:
	$(CC) -c $(CFLAGS) $(CEXTRA) $(DEFINCL) -o $@ $<

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CXXEXTRA) $(DEFINCL) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(CXXEXTRA) $(DEFINCL) -o $@ $<

.rc.res:
	$(RC) $(RCFLAGS) $(RCEXTRA) $(DEFINCL) -fo$@ $<

# Rules for cleaning

CLEAN_FILES     = y.tab.c y.tab.h lex.yy.c core *.orig *.rej \
                  \\\#*\\\# *~ *% .\\\#*

clean:: $(SUBDIRS:%=%/__clean__) $(EXTRASUBDIRS:%=%/__clean__)
	$(RM) $(CLEAN_FILES) $(RC_SRCS:.rc=.res) $(C_SRCS:.c=.o) $(CXX_SRCS:.cpp=.o)
	$(RM) $(DLLS:%=%.so) $(LIBS) $(EXES) $(EXES:%=%.so)

$(SUBDIRS:%=%/__clean__): dummy
	cd `dirname $@` && $(MAKE) clean

$(EXTRASUBDIRS:%=%/__clean__): dummy
	-cd `dirname $@` && $(RM) $(CLEAN_FILES)

### Target specific build rules
DEFLIB = $(LIBRARY_PATH) $(LIBRARIES) $(DLL_PATH) $(DLL_IMPORTS:%=-l%)

$(wav2at3_exe_MODULE): $(wav2at3_exe_OBJS)
	$(CXX) $(wav2at3_exe_LDFLAGS) -o $@ $(wav2at3_exe_OBJS) $(wav2at3_exe_LIBRARY_PATH) $(wav2at3_exe_DLL_PATH) $(DEFLIB) $(wav2at3_exe_DLLS:%=-l%) $(wav2at3_exe_LIBRARIES:%=-l%)


