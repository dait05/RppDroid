cmd_arch/arm/kernel/entry-armv.o := /home/benthian89/RVDroid/android-security/src-cm7sgt/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-gcc -Wp,-MD,arch/arm/kernel/.entry-armv.o.d  -nostdinc -isystem /home/benthian89/RVDroid/android-security/src-cm7sgt/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include -Iinclude  -I/home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-goldfish/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -msoft-float     -c -o arch/arm/kernel/entry-armv.o arch/arm/kernel/entry-armv.S

deps_arch/arm/kernel/entry-armv.o := \
  arch/arm/kernel/entry-armv.S \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/local/timers.h) \
    $(wildcard include/config/kprobes.h) \
    $(wildcard include/config/aeabi.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/needs/syscall/for/cmpxchg.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/neon.h) \
    $(wildcard include/config/cpu/arm610.h) \
    $(wildcard include/config/cpu/arm710.h) \
    $(wildcard include/config/iwmmxt.h) \
    $(wildcard include/config/crunch.h) \
    $(wildcard include/config/vfp.h) \
    $(wildcard include/config/cpu/32v6k.h) \
    $(wildcard include/config/has/tls/reg.h) \
    $(wildcard include/config/qemu/trace.h) \
    $(wildcard include/config/arm/thumb.h) \
    $(wildcard include/config/tls/reg/emul.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/memory.h \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/dram/base.h) \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem.h) \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/const.h \
  arch/arm/mach-goldfish/include/mach/memory.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/sizes.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/glue.h \
    $(wildcard include/config/cpu/abrt/lv4t.h) \
    $(wildcard include/config/cpu/abrt/ev4.h) \
    $(wildcard include/config/cpu/abrt/ev4t.h) \
    $(wildcard include/config/cpu/abrt/ev5tj.h) \
    $(wildcard include/config/cpu/abrt/ev5t.h) \
    $(wildcard include/config/cpu/abrt/ev6.h) \
    $(wildcard include/config/cpu/abrt/ev7.h) \
    $(wildcard include/config/cpu/pabrt/ifar.h) \
    $(wildcard include/config/cpu/pabrt/noifar.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/vfpmacros.h \
    $(wildcard include/config/vfpv3.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/vfp.h \
  arch/arm/mach-goldfish/include/mach/entry-macro.S \
  arch/arm/mach-goldfish/include/mach/hardware.h \
  arch/arm/mach-goldfish/include/mach/irqs.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/thread_notify.h \
  arch/arm/kernel/entry-header.S \
    $(wildcard include/config/frame/pointer.h) \
    $(wildcard include/config/alignment/trap.h) \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/linkage.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/linkage.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/ptrace.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/hwcap.h \
  include/asm/asm-offsets.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/arm/thumbee.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/fpstate.h \

arch/arm/kernel/entry-armv.o: $(deps_arch/arm/kernel/entry-armv.o)

$(deps_arch/arm/kernel/entry-armv.o):
