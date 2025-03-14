1.
Performance Benefits for Qt 5.6 Embedded Systems
This split approach is ideal for embedded systems like the Raspberry Pi running Qt 5.6:

Memory efficiency - Recursion uses more stack space, which is limited on embedded devices
CPU optimization - The memory calculation runs frequently and doesn't need recursion's overhead
Two-phase design - Collect textures once (expensive), calculate memory many times (cheap)

2.
