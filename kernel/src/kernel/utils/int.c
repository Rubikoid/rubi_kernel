#include <types.h>

#include <kernel/asm_lib.h>
#include <kernel/utils/int.h>
#include <kernel/vga/vga.h>

void abort(char *death_message) {
    vga_state.term_buffer = (uint16_t *)(0xC0000000 + 0xB8000);
    vga_state.term_col = 0;
    vga_state.term_row = 0;
    vga_state.term_color = V_BLACK << 4 | V_WHITE;

    term_print(death_message);
    disable_int();
    halt();
}

void cint_double_fail() {
    abort("ABORT: Double fail");
}

void cint_general_protect() {
    abort("ABORT: General protect");
}

void cint_aligment_check() {
    abort("ABORT: Aligment check");
}

void cint_division_by_zero() {
    abort("ABORT: Division by zero");
}

void cint_segment_not_present() {
    abort("ABORT: segment not present");
}

void cint_page_fault() {
    abort("ABORT: page fault");
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
