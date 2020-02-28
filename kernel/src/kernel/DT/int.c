#include <kernel/DT/int.h>
#include <kernel/asm_lib.h>
#include <kernel/messages.h>
#include <kernel/scheduler/task.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>
#include <types.h>

void cint_double_fail(PUSHAD_C) {
    kpanic(G_RED "Kernel panic: Double fail");
}

void cint_general_protect(uint16_t cs, PUSHAD_C, uint32_t error_code, uint32_t in_eip) {
    kpanic(G_RED "Kernel panic: General protect: CS: %x, EIP: %x, Error_code: %x (%x %x)", cs, in_eip, error_code, error_code >> 3, error_code & 0b110);
}

void cint_aligment_check(PUSHAD_C) {
    kpanic(G_RED "Kernel panic: Aligment check");
}

void cint_division_by_zero(PUSHAD_C) {
    kpanic(G_RED "Kernel panic: Division by zero");
}

void cint_segment_not_present(PUSHAD_C) {
    kpanic(G_RED "Kernel panic: segment not present");
}

void cint_page_fault(size_t addr, PUSHAD_C, uint32_t error_code, uint32_t in_eip) {
    uint32_t* prev_eip = in_ebp + 4;  // i belive there shuild be a next eip
    kpanic(G_RED
           "Kernel panic: "
           "page fault at %x, "
           "EIP: %x, "
           "Error_code:%x\n"
           "Prev: %x",
           addr,
           in_eip,
           error_code,
           *prev_eip);
}

static uint32_t counter = 0;

// just handle the tmier and do't do anything
void cint_timer(size_t* ret_addr, size_t* reg_addr, PUSHAD_C) {
    outb(PIC1_CMD_PORT, PIC_EOI);
    if (counter % 18 == 0)
        printf(MSG_SCH_UPTIME, counter / 18);
    counter++;
    sched_schedule(ret_addr, reg_addr);
}

void pic_init() {
    outb(PIC1_CMD_PORT, ICW1_INIT | ICW1_ICW4); /* init PIC1 */
    outb(PIC2_CMD_PORT, ICW1_INIT | ICW1_ICW4); /* init PIC2 */

    outb(PIC1_DATA_PORT, PIC1_INT_OFFSET); /* IQW2 offset */
    outb(PIC2_DATA_PORT, PIC2_INT_OFFSET); /* IQW2 offset */

    outb(PIC1_DATA_PORT, 0x00); /* IQW3 no cascade */
    outb(PIC2_DATA_PORT, 0x00); /* IQW3 no cascade */

    outb(PIC1_DATA_PORT, 0x01); /* IQW4 no cascade */
    outb(PIC2_DATA_PORT, 0x01); /* IQW4 no cascade */

    outb(PIC1_DATA_PORT, 0xff); /* disable irq PIC1 */
    outb(PIC2_DATA_PORT, 0xff); /* disable irq PIC2 */
}

void pic_enable() {
    outb(PIC1_DATA_PORT, 0xFC); /* Enable IRQ0, IRQ1 */

    // io_wait();
    // uint8_t x1 = inb(KB_PORT);
}

void dev_each_low_ih_cb(struct dev_t* entry, void* data) {
    struct ih_low_data_t* low_data = data;
    struct clist_head_t* current = entry->ih_list.head;
    struct ih_low_t* ih_low;

    //for (current = ; current != null; current = current->next) {
    do {
        ih_low = (struct ih_low_t*)current->data;
        if (ih_low->number == low_data->number) {
            if ((ih_low->subnumber & low_data->subnumber) != 0)
                ih_low->handler(low_data->number, low_data);
        }
    } while (current != entry->ih_list.head && current != NULL);
}
