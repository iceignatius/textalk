# ----------------------------------------------------------
# ---- Text Communication Library --------------------------
# ----------------------------------------------------------

# Detect OS name
ifeq ($(OS),)
	OS := $(shell uname -s)
endif

# Tools setting
CC  := gcc
CXX := g++
LD  := gcc
AR  := ar rcs

# Setting
OUTDIR  := .
OUTPUT := $(OUTDIR)/libtextalk.a
TEMPDIR := temp
INCDIR  :=
INCDIR  += -I../submod/genutil
INCDIR  += -I../include
LIBDIR  :=
CFLAGS  :=
CFLAGS  += -Wall
CFLAGS  += -O3
ifeq ($(OS),Windows_NT)
	CFLAGS += -D__USE_MINGW_ANSI_STDIO=1
endif
LDFLAGS :=
LDFLAGS += -s
SRCS    :=
SRCS    += ../submod/genutil/gen/systime.c
SRCS    += ../submod/genutil/gen/timeinf.c
SRCS    += src/textalk_conf.c
SRCS    += src/textalk_packet.c
SRCS    += src/textalk.c
LIBS    :=
OBJS    := $(notdir $(SRCS))
OBJS    := $(addprefix $(TEMPDIR)/,$(OBJS))
OBJS    := $(OBJS:%.c=%.o)
OBJS    := $(OBJS:%.cpp=%.o)
DEPS    := $(OBJS:%.o=%.d)

# Process summary
.PHONY: all clean
.PHONY: pre_step create_dir build_step post_step
.PHONY: install test
all: pre_step create_dir build_step post_step

# Clean process
clean:
ifeq ($(OS),Windows_NT)
	-del /Q $(subst /,\,$(OBJS))
	-del /Q $(subst /,\,$(DEPS))
	-del /Q $(subst /,\,$(OUTPUT))
	-rmdir /Q $(subst /,\,$(TEMPDIR))
else
	-@rm -f $(OBJS) $(DEPS) $(OUTPUT)
	-@rmdir $(TEMPDIR)
endif

# Build process

pre_step:
create_dir:
ifeq ($(OS),Windows_NT)
	@cmd /c if not exist $(subst /,\,$(TEMPDIR)) mkdir $(subst /,\,$(TEMPDIR))
	@cmd /c if not exist $(subst /,\,$(OUTDIR)) mkdir $(subst /,\,$(OUTDIR))
else
	@test -d $(TEMPDIR) || mkdir $(TEMPDIR)
	@test -d $(OUTDIR)  || mkdir $(OUTDIR)
endif
build_step: $(OUTPUT)
post_step:

$(OUTPUT): $(OBJS)
	$(AR) $@ $^

define Compile-C-Unit
$(CC) -MM $(INCDIR) $(CFLAGS) -o $(TEMPDIR)/$*.d $< -MT $@
$(CC) -c  $(INCDIR) $(CFLAGS) -o $@ $<
endef
define Compile-Cpp-Unit
$(CXX) -MM $(INCDIR) $(CFLAGS) -o $(TEMPDIR)/$*.d $< -MT $@
$(CXX) -c  $(INCDIR) $(CFLAGS) -o $@ $<
endef

-include $(DEPS)
$(TEMPDIR)/%.o: ../submod/genutil/gen/%.c
	$(Compile-C-Unit)
$(TEMPDIR)/%.o: ../submod/genutil/gen/%.cpp
	$(Compile-Cpp-Unit)
$(TEMPDIR)/%.o: src/%.c
	$(Compile-C-Unit)
$(TEMPDIR)/%.o: src/%.cpp
	$(Compile-Cpp-Unit)

# User extended process

install:

uninstall:

test: all
