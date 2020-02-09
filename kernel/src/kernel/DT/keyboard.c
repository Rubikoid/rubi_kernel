#include <kernel/DT/int.h>
#include <kernel/DT/keyboard.h>
#include <kernel/asm_lib.h>
#include <kernel/messages.h>
#include <kernel/scheduler/task.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>
#include <types.h>

void cint_keyboard(PUSHAD_C) {
    struct keyboard_status_t status;
    status.status = inb(KB_CMD_PORT);
    /*
        & 0x1: Output buffer status (0 = empty, 1 = full) 
            (must be set before attempting to read data from IO port 0x60)
        & 0x2: Input buffer status (0 = empty, 1 = full) 
            (must be clear before attempting to write data to IO port 0x60 or IO port 0x64)
        & 0x4: System Flag 
            Meant to be cleared on reset and set by firmware (via. PS/2 Controller Configuration Byte) if the system passes self tests (POST)
        & 0x8: Command/data
            (0 = data written to input buffer is data for PS/2 device, 1 = data written to input buffer is data for PS/2 controller command)
        & 0x10: May be "keyboard lock" (more likely unused on modern systems)
        & 0x20: May be "receive time-out" or "second PS/2 port output buffer full"
        & 0x40: Time-out error (0 = no error, 1 = time-out error)
        & 0x80: Parity error (0 = no error, 1 = parity error)
    */
    if (status.status & 0x1) {
        status.keycode = inb(KB_PORT);
        if (status.keycode >= 1) {
            struct ih_low_data_t ih_low_data;
            ih_low_data.number = INT_KEYBOARD;
            ih_low_data.data = &status;

            if (status.keycode == KB_DATA_SELFTEST_OK ||     // Self test passed (sent after "0xFF (reset)" command or keyboard power up)
                status.keycode == KB_DATA_ECHO ||            // Response to "0xEE (echo)" command
                status.keycode == KB_DATA_ACK ||             // Command acknowledged (ACK)
                status.keycode == KB_DATA_SELFTEST_FAIL1 ||  // Self test failed (sent after "0xFF (reset)" command or keyboard power up)
                status.keycode == KB_DATA_SELFTEST_FAIL2 ||  // Self test failed (sent after "0xFF (reset)" command or keyboard power up)
                status.keycode == KB_DATA_RESEND ||          // Resend (keyboard wants controller to repeat last command it sent)
                status.keycode == KB_DATA_ERROR)             // Key detection error or internal buffer overrun
                ih_low_data.subnumber = INT_KEYBOARD_NEW_DATA;
            else
                ih_low_data.subnumber = INT_KEYBOARD_NEW_KEY;

            dev_for_each(dev_each_low_ih_cb, &ih_low_data);  // we walk over the all driver list with O(n), and this is slow.
            // printf("Keyboard interrupt: %u 0x%x\n", keycode, keycode);
        }
    }
    outb(PIC1_CMD_PORT, PIC_EOI);
}
