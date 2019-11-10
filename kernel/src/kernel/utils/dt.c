#include <types.h>

#include <kernel/asm_lib.h>
#include <kernel/defines.h>
#include <kernel/utils/dt.h>
#include <kernel/utils/int.h>

struct GDT_raw_entry_t gdt[GDT_ENTRIES_COUNT];
struct GDT_normal_entry_t normal_gdt[GDT_ENTRIES_COUNT];
struct GDT_pointer_t gdt_ptr;

struct IDT_entry_t idt_table[IDT_ENTRIES_COUNT];

void gdt_init() {
    gdt_ptr.base = (uint32_t)&gdt;
    gdt_ptr.limit = GDT_ENTRIES_COUNT * sizeof(struct GDT_raw_entry_t) - 1;

    normal_gdt[GDT_NULL_SEGMENT].base = 0;
    normal_gdt[GDT_NULL_SEGMENT].limit = 0;
    normal_gdt[GDT_NULL_SEGMENT].type = 0;
    normal_gdt[GDT_NULL_SEGMENT].p = 0;
    normal_gdt[GDT_NULL_SEGMENT].a = 0;
    normal_gdt[GDT_NULL_SEGMENT].g = 0;
    normal_gdt[GDT_NULL_SEGMENT].s = 0;
    normal_gdt[GDT_NULL_SEGMENT].dpl = 0;
    normal_gdt[GDT_NULL_SEGMENT].db = 0;

    // kernel
    normal_gdt[GDT_KCODE_SEGMENT].base = 0;
    normal_gdt[GDT_KCODE_SEGMENT].limit = 0xFFFFF;
    normal_gdt[GDT_KCODE_SEGMENT].type = 0b1010;
    normal_gdt[GDT_KCODE_SEGMENT].p = 1;
    normal_gdt[GDT_KCODE_SEGMENT].a = 0;
    normal_gdt[GDT_KCODE_SEGMENT].g = 1;
    normal_gdt[GDT_KCODE_SEGMENT].s = 1;
    normal_gdt[GDT_KCODE_SEGMENT].dpl = 0;
    normal_gdt[GDT_KCODE_SEGMENT].db = 1;

    normal_gdt[GDT_KDATA_SEGMENT].base = 0;
    normal_gdt[GDT_KDATA_SEGMENT].limit = 0xFFFFF;
    normal_gdt[GDT_KDATA_SEGMENT].type = 0b0010;
    normal_gdt[GDT_KDATA_SEGMENT].p = 1;
    normal_gdt[GDT_KDATA_SEGMENT].a = 0;
    normal_gdt[GDT_KDATA_SEGMENT].g = 1;
    normal_gdt[GDT_KDATA_SEGMENT].s = 1;
    normal_gdt[GDT_KDATA_SEGMENT].dpl = 0;
    normal_gdt[GDT_KDATA_SEGMENT].db = 1;

    normal_gdt[GDT_KSTACK_SEGMENT].base = 0;
    normal_gdt[GDT_KSTACK_SEGMENT].limit = 0xFFFFFFFF;
    normal_gdt[GDT_KSTACK_SEGMENT].type = 0b0010;
    normal_gdt[GDT_KSTACK_SEGMENT].p = 1;
    normal_gdt[GDT_KSTACK_SEGMENT].a = 0;
    normal_gdt[GDT_KSTACK_SEGMENT].g = 0;
    normal_gdt[GDT_KSTACK_SEGMENT].s = 1;
    normal_gdt[GDT_KSTACK_SEGMENT].dpl = 0;
    normal_gdt[GDT_KSTACK_SEGMENT].db = 1;

    // user
    normal_gdt[GDT_UCODE_SEGMENT].base = 0;
    normal_gdt[GDT_UCODE_SEGMENT].limit = 0xFFFFFFFF;
    normal_gdt[GDT_UCODE_SEGMENT].type = 0b1010;
    normal_gdt[GDT_UCODE_SEGMENT].p = 1;
    normal_gdt[GDT_UCODE_SEGMENT].a = 0;
    normal_gdt[GDT_UCODE_SEGMENT].g = 0;
    normal_gdt[GDT_UCODE_SEGMENT].s = 1;
    normal_gdt[GDT_UCODE_SEGMENT].dpl = 0b11;
    normal_gdt[GDT_UCODE_SEGMENT].db = 1;

    normal_gdt[GDT_UDATA_SEGMENT].base = 0;
    normal_gdt[GDT_UDATA_SEGMENT].limit = 0xFFFFFFFF;
    normal_gdt[GDT_UDATA_SEGMENT].type = 0b0010;
    normal_gdt[GDT_UDATA_SEGMENT].p = 1;
    normal_gdt[GDT_UDATA_SEGMENT].a = 0;
    normal_gdt[GDT_UDATA_SEGMENT].g = 0;
    normal_gdt[GDT_UDATA_SEGMENT].s = 1;
    normal_gdt[GDT_UDATA_SEGMENT].dpl = 0b11;
    normal_gdt[GDT_UDATA_SEGMENT].db = 1;

    normal_gdt[GDT_USTACK_SEGMENT].base = 0;
    normal_gdt[GDT_USTACK_SEGMENT].limit = 0xFFFFFFFF;
    normal_gdt[GDT_USTACK_SEGMENT].type = 0b0010;
    normal_gdt[GDT_USTACK_SEGMENT].p = 1;
    normal_gdt[GDT_USTACK_SEGMENT].a = 0;
    normal_gdt[GDT_USTACK_SEGMENT].g = 0;
    normal_gdt[GDT_USTACK_SEGMENT].s = 1;
    normal_gdt[GDT_USTACK_SEGMENT].dpl = 0b11;
    normal_gdt[GDT_USTACK_SEGMENT].db = 1;

    gdt_flush();
    gdt_load(&gdt_ptr);
}

void gdt_flush() {
    for (int i = 0; i < GDT_ENTRIES_COUNT; i++) {
        encode_gdt_entry(&gdt[i], &normal_gdt[i]);
    }
}

void encode_gdt_entry(struct GDT_raw_entry_t *target, struct GDT_normal_entry_t *source) {
    // Check the limit to make sure that it can be encoded
    if ((source->limit > 65536) && ((source->limit & 0xFFF) != 0xFFF)) {
        return;
        //kerror("You can't do that!");
    }
    if (source->limit > 65536 && !source->g) {
        // Adjust granularity if required
        source->limit = source->limit >> 12;
        target->g = 1;
    } else if (source->g) {
        target->g = 1;
    } else {
        target->g = 0;
    }

    target->limit_low = source->limit & 0xFFFF;
    target->limit_high = (source->limit >> 16) & 0xF;

    target->base_low = source->base & 0xFFFF;
    target->base_middle = (source->base >> 16) & 0xFF;
    target->base_high = (source->base >> 24) & 0xFF;

    target->type = source->type;

    target->s = source->s;
    target->dpl = source->dpl;
    target->p = source->p;

    target->a = source->s;
    target->zero = 0;
    target->db = source->db;
}

void idt_init() {
    size_t idt_address;
    size_t idt_ptr[2];

    pic_init();

    idt_fill_entry(INT_DIVIDE_BY_ZERO, (size_t)cint_division_by_zero);
    idt_fill_entry(INT_DOUBLE_FAULT, (size_t)cint_double_fail);
    idt_fill_entry(INT_SEGMENT_NOT_PRES, (size_t)cint_segment_not_present);
    idt_fill_entry(INT_GENERAL_PROTECT, (size_t)cint_general_protect);
    idt_fill_entry(INT_ALIGNMENT_CHECK, (size_t)cint_aligment_check);
    idt_fill_entry(INT_PAGE_FAULT, (size_t)cint_page_fault);

    idt_address = (size_t)&idt_table;
    idt_ptr[0] = (sizeof(struct IDT_entry_t) * IDT_ENTRIES_COUNT) + ((idt_address & 0xFFFF) << 16);
    idt_ptr[1] = idt_address >> 16;
    idt_load((unsigned long *)idt_ptr);
    DEBUG_ASM;
}

void idt_fill_entry(uint8_t offset, size_t handler) {
    idt_table[offset].offset_lowerbits = handler & 0xFFFF;
    idt_table[offset].selector = GDT_KCODE_SELECTOR;
    idt_table[offset].zero = 0;
    idt_table[offset].type_attr = INTERRUPT_GATE;
    idt_table[offset].offset_higherbits = handler >> 16;
}
