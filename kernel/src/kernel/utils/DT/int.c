#include <types.h>

#include <kernel/asm_lib.h>
#include <kernel/utils/DT/int.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>

void cint_double_fail(PUSHAD_C) {
    kpanic("Kernel panic: Double fail");
}

void cint_general_protect(uint16_t cs, PUSHAD_C, uint32_t error_code, uint32_t in_eip) {
    kpanic("Kernel panic: General protect: CS: %x, EIP: %x, Error_code: %x (%x %x)", cs, in_eip, error_code, error_code >> 3, error_code & 0b110);
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

void cint_page_fault(size_t addr, PUSHAD_C, uint32_t error_code, uint32_t in_eip) {
    kpanic("Kernel panic: page fault at %x, EIP: %x, Error_code: %x", addr, in_eip, error_code);
}

// just handle the tmier and do't do anything
void cint_timer(PUSHAD_C) {
    out_byte(PIC1_CMD_PORT, PIC_EOI);
}

void cint_keyboard(PUSHAD_C) {
    uint8_t status = in_byte(KB_CMD_PORT);
    if(status & 0x1) {
        uint8_t keycode = in_byte(KB_PORT);
        if(keycode >= 1) {
            // printf("Keyboard interrupt: %u 0x%x\n", keycode, keycode);
        }
    }
    out_byte(PIC1_CMD_PORT, PIC_EOI);
}

void pic_init() {
    out_byte(PIC1_CMD_PORT, ICW1_INIT | ICW1_ICW4); /* init PIC1 */
    out_byte(PIC2_CMD_PORT, ICW1_INIT | ICW1_ICW4); /* init PIC2 */

    out_byte(PIC1_DATA_PORT, PIC1_INT_OFFSET); /* IQW2 offset */
    out_byte(PIC2_DATA_PORT, PIC2_INT_OFFSET); /* IQW2 offset */

    out_byte(PIC1_DATA_PORT, 0x00); /* IQW3 no cascade */
    out_byte(PIC2_DATA_PORT, 0x00); /* IQW3 no cascade */

    out_byte(PIC1_DATA_PORT, 0x01); /* IQW4 no cascade */
    out_byte(PIC2_DATA_PORT, 0x01); /* IQW4 no cascade */

    out_byte(PIC1_DATA_PORT, 0xff); /* disable irq PIC1 */
    out_byte(PIC2_DATA_PORT, 0xff); /* disable irq PIC2 */
}

void pic_enable() {
    out_byte(PIC1_DATA_PORT, 0xFC); /* Enable IRQ0, IRQ1 */

    // io_wait();
    // uint8_t x1 = in_byte(KB_PORT);
}
