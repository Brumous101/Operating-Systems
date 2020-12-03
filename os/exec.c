asm volatile(
    "mrs r0,cpsr\n"
    "and r0,#0xffffffe0\n"
    "orr r0,#0x10\n"
    "msr cpsr,r0\n"
    "mov sp,#0x800000\n"
    "mov pc,#0x400000\n"
    : : : "memory"
);

