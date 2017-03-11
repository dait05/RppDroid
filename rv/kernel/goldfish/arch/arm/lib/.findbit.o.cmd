cmd_arch/arm/lib/findbit.o := /home/benthian89/RVDroid/android-security/src-cm7sgt/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-gcc -Wp,-MD,arch/arm/lib/.findbit.o.d  -nostdinc -isystem /home/benthian89/RVDroid/android-security/src-cm7sgt/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include -Iinclude  -I/home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-goldfish/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -msoft-float     -c -o arch/arm/lib/findbit.o arch/arm/lib/findbit.S

deps_arch/arm/lib/findbit.o := \
  arch/arm/lib/findbit.S \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/linkage.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
    $(wildcard include/config/smp.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/hwcap.h \

arch/arm/lib/findbit.o: $(deps_arch/arm/lib/findbit.o)

$(deps_arch/arm/lib/findbit.o):
