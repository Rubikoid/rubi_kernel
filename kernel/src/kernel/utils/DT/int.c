#include <types.h>

#include <kernel/asm_lib.h>
#include <kernel/utils/DT/int.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>

void cint_double_fail(PUSHAD_C) {
    kpanic("Kernel panic: Double fail");
}

void cint_general_protect(PUSHAD_C) {
    kpanic("Kernel panic: General protect");
}

void cint_aligment_check(PUSHAD_C) {
    kpanic("Kernel panic: Aligment check");
}

void cint_division_by_zero(PUSHAD_C) {
    kpanic("Kernel panic: Division by zero");
}

void cint_segment_not_present(PUSHAD_C) {
    kpanic("Kernel panic: segment not present");
}

void cint_page_fault(size_t addr, PUSHAD_C) {
    kpanic("Kernel panic: page fault at 0x%x", addr);
}

void pic_init() {
    out_byte(PIC1_CMD_PORT, PIC_IQW_CMD); /* init PIC1 */
    out_byte(PIC2_CMD_PORT, PIC_IQW_CMD); /* init PIC2 */

    out_byte(PIC1_DATA_PORT, 0x20); /* IQW2 offset */
    out_byte(PIC2_DATA_PORT, 0x28); /* IQW2 offset */

    out_byte(PIC1_DATA_PORT, 0x00); /* IQW3 no cascade */
    out_byte(PIC2_DATA_PORT, 0x00); /* IQW3 no cascade */

    out_byte(PIC1_DATA_PORT, 0x01); /* IQW4 no cascade */
    out_byte(PIC2_DATA_PORT, 0x01); /* IQW4 no cascade */

    out_byte(PIC1_DATA_PORT, 0xff); /* disable irq PIC1 */
    out_byte(PIC2_DATA_PORT, 0xff); /* disable irq PIC2 */
}

void pic_enable() {
    out_byte(PIC1_DATA_PORT, 0xFC); /* Enable IRQ0, IRQ1 */
}
