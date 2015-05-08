# Read .config first (if any) and use those values
# But we must remove the quotes from these Kconfig values
-include $(CURDIR)/.config
ARCH ?= $(patsubst "%",%,$(CONFIG_ARCH))
CROSS_COMPILE ?= $(patsubst "%",%,$(CONFIG_CROSS_COMPILE))
MODE ?= $(patsubst "%",%,$(CONFIG_MEMORY_MODE))

# if no .config is there, ARCH is still empty, this would prevent a simple
# "make config"
ifeq ($(ARCH),)
  ARCH = lpc1343
endif

# First: the target. After that, we can include the arch Makefile
all: bathos.bin

ADIR = arch-$(ARCH)
include $(ADIR)/Makefile

# Task choice. This follows the -y convention, to allow use of $(CONFIG_STH) 
# The arch may have its choice, or you can override on the command line
TASK-y ?= task-uart.o arch/task-led.o arch/task-pwm.o

# Cross compiling:
AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
CPP             = $(CC) -E
AR              = $(CROSS_COMPILE)ar
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump

# Only arch-unix needs $(CC) for final link
ARCH-FINAL-LD  ?= $(LD)

# Files that depend on the architecture (bathos.lds may be missing)
AOBJ  = $(ADIR)/boot.o $(ADIR)/io.o

# The user can pass USER_CFLAGS if needed
CFLAGS += $(USER_CFLAGS)

# Kconfig output
CFLAGS += -include include/generated/autoconf.h
ASFLAGS += -include include/generated/autoconf.h

# There may or may not be a linker script (arch-unix doesn't)
LDS   = $(wildcard $(ADIR)/bathos$(MODE).lds)

# Lib objects and flags
LOBJ-y = pp_printf/printf.o
LOBJ-$(CONFIG_PRINTF_FULL) += pp_printf/vsprintf-full.o
LOBJ-$(CONFIG_PRINTF_XINT) += pp_printf/vsprintf-xint.o

CFLAGS  += -Ipp_printf -DCONFIG_PRINT_BUFSIZE=256

# Use our own linker script, if it exists
LDFLAGS += $(patsubst %.lds, -T %.lds, $(LDS))

# Each architecture can have specific drivers
LDFLAGS += $(LIBARCH)

# We have drivers too, let its Makefile do it all
include drivers/Makefile

# Same for the generic library
include lib/Makefile

# As the system goes larger, we need libgcc to resolve missing symbols
LDFLAGS += $(shell $(CC) $(CFLAGS) --print-libgcc-file-name)


# Task object files. All objects are placed in tasks/ but the source may
# live in tasks-$(ARCH), to allow similar but different implementations
TOBJ := $(patsubst %, tasks/%, $(TASK-y))
TOBJ := $(patsubst tasks/arch/%, tasks-$(ARCH)/%, $(TOBJ))
# Finally, some tasks are specifically in tests/
TOBJ := $(patsubst tasks/test-%, tests/test-%, $(TOBJ))
VPATH := tasks-$(ARCH)

# Generic flags
CFLAGS  += -Iinclude -I$(ADIR)
ASFLAGS  += -Iinclude -I$(ADIR)
CFLAGS  += -g -Wall -ffreestanding -Os
ASFLAGS += -g -Wall

# Our target
bathos.bin: bathos
	$(OBJCOPY) -O binary $^ $@

# Final link is done with $(LD) or $(CC)
bathos: bathos.o
	$(ARCH-FINAL-LD) bathos.o $(LDFLAGS) -o $@

obj-y =  main.o init.o $(AOBJ) $(TOBJ) $(LOBJ-y) $(LIBARCH) $(LIBS)

bathos.o: silentoldconfig $(obj-y)
	$(LD) -r -T bigobj.lds $(obj-y) -o $@

clean:
	rm -f bathos.bin bathos *.o *~
	find . -name '*.o' -o -name '*~' -o -name '*.a' | \
		grep -v scripts/kconfig | xargs rm -f

# following targets from Makefile.kconfig
silentoldconfig:
	@mkdir -p include/config
	$(MAKE) -f Makefile.kconfig $@

scripts_basic config:
	$(MAKE) -f Makefile.kconfig $@

%config:
	$(MAKE) -f Makefile.kconfig $@

defconfig:
	@echo "Using lpc1343_defconfig"
	@test -f .config || touch .config
	@$(MAKE) -f Makefile.kconfig lpc1343_defconfig

.config: silentoldconfig
