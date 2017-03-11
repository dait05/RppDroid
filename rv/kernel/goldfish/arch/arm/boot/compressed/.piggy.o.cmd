cmd_arch/arm/boot/compressed/piggy.o := /home/benthian89/RVDroid/android-security/src-cm7sgt/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-gcc -Wp,-MD,arch/arm/boot/compressed/.piggy.o.d  -nostdinc -isystem /home/benthian89/RVDroid/android-security/src-cm7sgt/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include -Iinclude  -I/home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-goldfish/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -msoft-float  -Wa,-march=all   -c -o arch/arm/boot/compressed/piggy.o arch/arm/boot/compressed/piggy.S

deps_arch/arm/boot/compressed/piggy.o := \
  arch/arm/boot/compressed/piggy.S \

arch/arm/boot/compressed/piggy.o: $(deps_arch/arm/boot/compressed/piggy.o)

$(deps_arch/arm/boot/compressed/piggy.o):
