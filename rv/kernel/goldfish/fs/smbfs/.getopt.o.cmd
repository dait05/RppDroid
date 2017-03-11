cmd_fs/smbfs/getopt.o := /home/benthian89/RVDroid/android-security/src-cm7sgt/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-gcc -Wp,-MD,fs/smbfs/.getopt.o.d  -nostdinc -isystem /home/benthian89/RVDroid/android-security/src-cm7sgt/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include -Iinclude  -I/home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-goldfish/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Os -marm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fno-omit-frame-pointer -fno-optimize-sibling-calls -Wdeclaration-after-statement -Wno-pointer-sign -fwrapv -fno-dwarf2-cfi-asm -DSMBFS_PARANOIA  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(getopt)"  -D"KBUILD_MODNAME=KBUILD_STR(smbfs)"  -c -o fs/smbfs/.tmp_getopt.o fs/smbfs/getopt.c

deps_fs/smbfs/getopt.o := \
  fs/smbfs/getopt.c \
  include/linux/kernel.h \
    $(wildcard include/config/lbd.h) \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/printk/debug.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stdarg.h \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/linkage.h \
  include/linux/stddef.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  include/linux/posix_types.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/posix_types.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/bitops.h \
    $(wildcard include/config/smp.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/system.h \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/sa110.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
    $(wildcard include/config/x86.h) \
  include/linux/typecheck.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/irqflags.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/hwcap.h \
  include/asm-generic/cmpxchg-local.h \
  include/asm-generic/cmpxchg.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/lock.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/ratelimit.h \
  include/linux/param.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/param.h \
    $(wildcard include/config/hz.h) \
  include/linux/dynamic_printk.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  include/linux/string.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/string.h \
  include/linux/net.h \
    $(wildcard include/config/sysctl.h) \
  include/linux/socket.h \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/compat.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/socket.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/sockios.h \
  include/linux/sockios.h \
  include/linux/uio.h \
  include/linux/stringify.h \
  include/linux/random.h \
  include/linux/ioctl.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/linux/irqnr.h \
    $(wildcard include/config/generic/hardirqs.h) \
  include/linux/wait.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
  include/linux/prefetch.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/processor.h \
    $(wildcard include/config/mmu.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/cache.h \
    $(wildcard include/config/arm/l1/cache/shift.h) \
    $(wildcard include/config/aeabi.h) \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/arm/thumbee.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/iwmmxt.h) \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  include/linux/spinlock_types_up.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lock/stat.h) \
  include/linux/spinlock_up.h \
  include/linux/spinlock_api_up.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/atomic.h \
  include/asm-generic/atomic.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/current.h \
  include/linux/fcntl.h \
  /home/benthian89/RVDroid/android-security/src-cm7sgt/kernel/goldfish/arch/arm/include/asm/fcntl.h \
  include/asm-generic/fcntl.h \
  include/linux/sysctl.h \
  fs/smbfs/getopt.h \

fs/smbfs/getopt.o: $(deps_fs/smbfs/getopt.o)

$(deps_fs/smbfs/getopt.o):
