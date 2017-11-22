TARGET    = account-manager

SRCDIR    = $(PWD)
LIBDIR    = $(PWD)/import
OUTDIR    = $(PWD)/output
OBJDIR    = $(PWD)/.obj


SRCS     +=  $(wildcard $(LIBDIR)/gorilla/log/*.cpp)
CFLAGS   += -I$(LIBDIR)/gorilla/log
SRCS     += $(wildcard $(LIBDIR)/gorilla/utility/*.cpp)
CFLAGS   += -I$(LIBDIR)/gorilla/utility
SRCS     += $(wildcard $(LIBDIR)/hash/*.c)
CFLAGS   += -I$(LIBDIR)/hash
SRCS     += $(wildcard $(LIBDIR)/jsoncpp/jsoncpp.cpp)
CFLAGS   += -I$(LIBDIR)/jsoncpp

LDFLAGS  += -licudata -licui18n -licutu -licuuc

CXXFLAGS += -I$(LIBDIR)
LDFLAGS  += -lcurl -lsqlite3

CXXFLAGS += -I$(LIBDIR)/websocketpp/include
CXXFLAGS += -I$(LIBDIR)/gorilla/http/include
LDFLAGS  += -L$(LIBDIR)/gorilla/http/lib
LDFLAGS  += -lgorilla_http

# Static link cpp-network-libs
CPPFLAGS += -DASIO_HEADER_ONLY -DBOOST_NETWORK_DEBUG
CXXFLAGS += -I$(LIBDIR)/cppnetlib/include
LDFLAGS  += -L$(LIBDIR)/cppnetlib
LDFLAGS  += -lcppnetlib-uri -lcppnetlib-server-parsers -lcppnetlib-client-connections

CXXFLAGS += -DBOOST_SPIRIT_USE_PHOENIX_V3
CXXFLAGS += -DBOOST_ALL_DYN_LINK
LDFLAGS  += -lboost_atomic -lboost_chrono -lboost_coroutine -lboost_context
LDFLAGS  += -lboost_date_time -lboost_exception -lboost_filesystem
LDFLAGS  += -lboost_iostreams -lboost_log -lboost_log_setup -lboost_locale
LDFLAGS  += -lboost_program_options -lboost_regex -lboost_system -lboost_thread

CXXFLAGS += -DBOOST_NETWORK_ENABLE_HTTPS
LDFLAGS  += -lcrypto -lssl

CXXFLAGS += -std=c++11
LDFLAGS  += -lpthread

LDFLAGS  += -lcurl

LIBPATH   = .


################################################################################
CC       := $(TOOLCHAIN_PREFIX)gcc
CXX      := $(TOOLCHAIN_PREFIX)g++
LD       := $(TOOLCHAIN_PREFIX)ld
AR       := $(TOOLCHAIN_PREFIX)ar

SRCS     += $(wildcard $(SRCDIR)/*.c $(SRCDIR)/*.cpp)
OBJS     += $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS)))

ifdef LIBPATH
LDFLAGS  += -Wl,-rpath,$(LIBPATH)
endif

all: CFLAGS += -O3
all: prebuild $(OUTDIR)/$(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: prebuild $(OUTDIR)/$(TARGET)

$(OUTDIR)/$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@-mkdir -p $(dir $@)
	$(CC) -o $@ -MMD -c $(CFLAGS) $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@-mkdir -p $(dir $@)
	$(CXX) -o $@ -MMD -c $(CFLAGS) $(CXXFLAGS) $<

clean:
	-rm -rf $(OUTDIR)/$(TARGET) $(OBJDIR)/$(OBJS)

distclean:
	-rm -rf $(OUTDIR) $(OBJS)

prebuild:
	-mkdir -p $(OUTDIR)
	-mkdir -p $(OBJDIR)
	-cp -a configs/* $(OUTDIR)

-include $(OBJS:.o=.d)
