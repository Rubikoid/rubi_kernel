#include <kernel/dev/dev.h>
#include <kernel/scheduler/task.h>
#include <lib/clist.h>
#include <lib/stdio.h>
#include <types.h>

#ifndef KERNEL_DEV_PCI_H_
#define KERNEL_DEV_PCI_H_

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

#define PCI_IOCTL_XXX (1 << 8)

#define PCI_CLASS_NAN         0x00
#define PCI_CLASS_MASS_STOR   0x01
#define PCI_CLASS_NETWORK     0x02
#define PCI_CLASS_DISP        0x03
#define PCI_CLASS_MULTIMED    0x04
#define PCI_CLASS_MEM         0x05
#define PCI_CLASS_BRIDGE      0x06
#define PCI_CLASS_SIMPL_COM   0x07
#define PCI_CLASS_BASE_PEREPH 0x08
#define PCI_CLASS_INPUT_DEV   0x09
#define PCI_CLASS_DOCK        0x0A
#define PCI_CLASS_PROCESSOR   0x0B
#define PCI_CLASS_SERIAL_BUS  0x0C
#define PCI_CLASS_WIRELESS    0x0D
#define PCI_CLASS_INTELL_CONT 0x0E
#define PCI_CLASS_SATELL_COMM 0x0F
#define PCI_CLASS_ENCRY_CONT  0x10
#define PCI_COUNT             0x11

/*
do_magic({
    'CLASS_NAN': '0x0',
    'CLASS_MASS_STOR': '0x1',
    'CLASS_NETWORK': '0x2',
    'CLASS_DISP': '0x3',
    'CLASS_MULTIMED': '0x4',
    'CLASS_MEM': '0x5',
    'CLASS_BRIDGE': '0x6',
    'CLASS_SIMPL_COM': '0x7',
    'CLASS_BASE_PEREPH': '0x8',
    'CLASS_INPUT_DEV': '0x9',
    'CLASS_DOCK': '0xA',
    'CLASS_PROCESSOR': '0xB',
    'CLASS_SERIAL_BUS': '0xC',
    'CLASS_WIRELESS': '0xD',
    'CLASS_INTELL_CONT': '0xE',
    'CLASS_SATELL_COMM': '0xF',
    'CLASS_ENCRY_CONT': '0x10',
}, pref="PCI")
*/

extern const char *pci_dev_name;

extern void pci_init();
extern void pci_ih_low(uint32_t number, struct ih_low_data_t *data);
extern void pci_ih_high(struct message_t *msg);

extern void pci_ioctl(uint32_t command, uint32_t subcm);

extern uint32_t pci_write(void *buf, uint32_t *offset, uint32_t size);
// extern void pci_write_ch(char ch);

extern uint32_t pci_read(void *buf, uint32_t *offset, uint32_t size);
//extern char pci_read_ch();

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pci_check_vendor(uint8_t bus, uint8_t slot);

#endif
