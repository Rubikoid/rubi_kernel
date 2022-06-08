#include <kernel/asm_lib.h>
#include <kernel/serial/serial.h>
#include <lib/stdio.h>

uint16_t select_serial(uint16_t com_id) {
    switch (com_id) {
        case 0:
            return COM1_PORT;
            break;
        case 1:
            return COM2_PORT;
            break;
        case 2:
            return COM3_PORT;
            break;
        case 4:
            return COM4_PORT;
            break;
        default:
            return COM1_PORT;
    }
}

int test_com(uint16_t com) {
    outb(com + COM_MODEM_CTRL, 0x1E);  // Set in loopback mode, test the serial chip
    outb(com + COM_DATA, 0xAE);        // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if (inb(com + COM_DATA) != 0xAE) {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(com + COM_MODEM_CTRL, 0x0F);
    return 0;
}

void init_com(uint16_t com_id) {
    uint16_t com = select_serial(com_id);

    outb(com + COM_INT, 0x00);         // Disable all interrupts
    outb(com + COM_D_MSB, 0x80);       // Enable DLAB (set baud rate divisor)
    outb(com + COM_D_LSB_DIV, 0x03);   // Set divisor to 3 (lo byte) 38400 baud
    outb(com + COM_D_MSB_DIV, 0x00);   //                  (hi byte)
    outb(com + COM_D_MSB, 0x03);       // 8 bits, no parity, one stop bit
    outb(com + COM_INT_FIFO, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(com + COM_MODEM_CTRL, 0x0B);  // IRQs enabled, RTS/DSR set
    outb(com + COM_INT, 0x01);         // Enable 'Data available' interrupt

    // if (!test_com(com)) {
    //     kpanic("COM %d check failed", com);
    // }
}

void write_com(uint16_t com_id, uint8_t data) {
    uint16_t com = select_serial(com_id);
    while ((inb(com + COM_LINE_STAT) & 0x20) == 0)
        ;
    outb(com + COM_DATA, data);
}

uint8_t read_com(uint16_t com_id, uint8_t blocking) {
    uint16_t com = select_serial(com_id);
    if (blocking) {
        while ((inb(com + COM_LINE_STAT) & 0x1) == 0)
            ;
    }
    return inb(com + COM_DATA);
}
