TARGET    = account-manager

SRCDIR    = $(PWD)
LIBDIR    = $(PWD)/import
OUTDIR    = $(PWD)/outputs
OBJDIR    = $(PWD)/.obj
BOOSTDIR  = $(PWD)/external/linux-x86_64/boost
OPENLDAPDIR = $(PWD)/external/linux-x86_64/openldap
CURLDIR = $(PWD)/external/linux-x86_64/curl
#OPENLDAP_2.4.49DIR = $(PWD)/external/linux-x86_64/openldap-2.4.49/openldapc

CXXFLAGS += -I$(BOOSTDIR)/include 
CXXFLAGS += -I$(OPENLDAPDIR)/include
CXXFLAGS += -I$(CURLDIR)/include
#CXXFLAGS += -I$(OPENLDAP_2.4.49DIR)
LDFLAGS  += -L$(BOOSTDIR)/lib
LDFLAGS  += -L$(OPENLDAPDIR)/lib
LDFLAGS  += -L$(CURLDIR)/lib
#LDFALGS  += -L$(OPENLDAP_2.4.49DIR)/libraries

CXXFLAGS += -DBOOST_NETWORK_NO_LIB
SRCS     +=  $(wildcard $(LIBDIR)/gorilla/log/*.cpp)
CFLAGS   += -I$(LIBDIR)/gorilla/log
SRCS     += $(wildcard $(LIBDIR)/gorilla/utility/*.cpp)
CFLAGS   += -I$(LIBDIR)/gorilla/utility
SRCS     += $(wildcard $(LIBDIR)/hash/*.c)
SRCS     += 
CFLAGS   += -I$(LIBDIR)/hash
SRCS     += $(wildcard $(LIBDIR)/jsoncpp/jsoncpp.cpp)
CFLAGS   += -I$(LIBDIR)/jsoncpp

#LDFLAGS  += -licudata -licui18n -licutu -licuuc

CXXFLAGS += -I$(LIBDIR)
LDFLAGS  += -lcurl -lsqlite3

CXXFLAGS += -I$(LIBDIR)/websocketpp/include
CXXFLAGS += -I$(LIBDIR)/gorilla/http/include
LDFLAGS  += -L$(LIBDIR)/gorilla/http/lib
LDFLAGS  += -lgorilla_http

# Static link cpp-network-libs
CPPFLAGS += -DASIO_HEADER_ONLY -DBOOST_NETWORK_DEBUG
CXXFLAGS += -I$(LIBDIR)/cppnetlib/include

CXXFLAGS += -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_LOG_DYN_LINK
LDFLAGS  += -lboost_chrono -lboost_context
LDFLAGS  += -lboost_date_time -lboost_filesystem
LDFLAGS  += -lboost_iostreams -lboost_log -lboost_log_setup -lboost_locale
LDFLAGS  += -lboost_program_options -lboost_regex -lboost_system -lboost_thread

CXXFLAGS += -DBOOST_NETWORK_ENABLE_HTTPS
LDFLAGS  += -lcrypto -lssl
LDFLAGS  += -lldapcpp -lldap -lsasl2
LDFLAGS  += -llber

CXXFLAGS += -std=c++11
LDFLAGS  += -lpthread


LIBPATH   = .

CONFIGPATH = configs
ifdef TELSTRA
  CONFIGPATH = configs/telstra
  CXXFLAGS += -DTELSTRA=1
endif


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
	-rm -f $(OBJDIR)/Config.*
	-mkdir -p $(OUTDIR)/default
	-mkdir -p $(OBJDIR)
	-cp $(CONFIGPATH)/config.json $(OUTDIR)
	-cp configs/settings.ini $(OUTDIR)/default
	-cp configs/*.pem $(OUTDIR)
	-cp -r packaging/systemd $(OUTDIR)
	-cp VERSION.txt $(OUTDIR)
	-cp configs/ldap_config.json $(OUTDIR)


-include $(OBJS:.o=.d)
