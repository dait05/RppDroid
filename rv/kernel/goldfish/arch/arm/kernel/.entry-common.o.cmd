cmd_arch/arm/kernel/entry-common.o := /home/benthian89/RVDroid/android-security/src-cm7sgt/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-gcc -Wp,-MD,arch/arm/kernel/.entry-common.o.d  -nostdinc -isystem /home/benthian89/RVDroid/android-security/src-cm7sgt/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include -Iinclude  -I/home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-goldfish/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -msoft-float     -c -o arch/arm/kernel/entry-common.o arch/arm/kernel/entry-common.S

deps_arch/arm/kernel/entry-common.o := \
  arch/arm/kernel/entry-common.S \
    $(wildcard include/config/function/tracer.h) \
    $(wildcard include/config/dynamic/ftrace.h) \
    $(wildcard include/config/cpu/arm710.h) \
    $(wildcard include/config/oabi/compat.h) \
    $(wildcard include/config/arm/thumb.h) \
    $(wildcard include/config/aeabi.h) \
    $(wildcard include/config/alignment/trap.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/unistd.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/ftrace.h \
  arch/arm/mach-goldfish/include/mach/entry-macro.S \
  arch/arm/mach-goldfish/include/mach/hardware.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/sizes.h \
  arch/arm/mach-goldfish/include/mach/irqs.h \
  arch/arm/kernel/entry-header.S \
    $(wildcard include/config/frame/pointer.h) \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/linkage.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/linkage.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/smp.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/hwcap.h \
  include/asm/asm-offsets.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/arm/thumbee.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/iwmmxt.h) \
  arch/arm/kernel/calls.S \

arch/arm/kernel/entry-common.o: $(deps_arch/arm/kernel/entry-common.o)

$(deps_arch/arm/kernel/entry-common.o):
