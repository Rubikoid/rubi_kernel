#include <kernel/DT/int.h>
#include <kernel/asm_lib.h>
#include <kernel/dev/pci.h>
#include <kernel/kthread/dq.h>
#include <kernel/kthread/ktasks.h>
#include <kernel/memory/heap.h>
#include <lib/string.h>

#define __MODULE_NAME__ "PCI"

const char *pci_dev_name = "PCI";

static const char *pci_names[PCI_COUNT] = {
    "CLASS_NAN",
    "CLASS_MASS_STOR",
    "CLASS_NETWORK",
    "CLASS_DISP",
    "CLASS_MULTIMED",
    "CLASS_MEM",
    "CLASS_BRIDGE",
    "CLASS_SIMPL_COM",
    "CLASS_BASE_PEREPH",
    "CLASS_INPUT_DEV",
    "CLASS_DOCK",
    "CLASS_PROCESSOR",
    "CLASS_SERIAL_BUS",
    "CLASS_WIRELESS",
    "CLASS_INTELL_CONT",
    "CLASS_SATELL_COMM",
    "CLASS_ENCRY_CONT",
};

void pci_init() {
    struct clist_head_t *entry;
    struct dev_t *dev;
    struct ih_low_t *ih_low;

    dev = dev_create();

    strncpy(dev->name, pci_dev_name, sizeof(dev->name) - 1);

    dev->base_r = 0;
    dev->base_w = 0;

    dev->read_fn = pci_read;  // TODO: implement
    dev->write_fn = pci_write;
    dev->cmd_fn = pci_ioctl;

    dev->ih_list.head = NULL;
    dev->ih_list.tail = NULL;
    dev->ih_list.slot_size = sizeof(struct ih_low_t);
    entry = clist_insert_after(&dev->ih_list, dev->ih_list.tail);

    ih_low = (struct ih_low_t *)entry->data;
    ih_low->number = 0;
    ih_low->subnumber = 0;
    ih_low->handler = pci_ih_low;
}

void pci_ih_low(uint32_t number, struct ih_low_data_t *data) {
    struct message_t msg;  // create update task
    msg.type = IPC_MSG_TYPE_DQ_SCHED;
    msg.len = 4;
    msg.data = kmalloc(4);
    *((size_t *)msg.data) = (size_t)pci_ih_high;
    ksend(ktasks[KERNEL_DQ_TASK_ID]->tid, &msg);
}

void pci_ih_high(struct message_t *msg) {
}

void pci_ioctl(uint32_t command, uint32_t subcm) {
    switch (command) {
        case IOCTL_INIT: {
            break;
        }
        case IOCTL_FLUSH: {
            break;
        }
        case PCI_IOCTL_XXX: {
            break;
        }
        default: {
            break;
        }
    }
}

uint32_t pci_write(void *buf, uint32_t *offset, uint32_t size) {
    return 0;
}

/*
void pci_write_ch(char ch) {
}
*/

uint32_t pci_read(void *buf, uint32_t *offset, uint32_t size) {
    return 0;
}

/*
char pci_read_ch() {
    return '\0';
}
*/

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t ret = 0;

    address = (uint32_t)((lbus << 16) | (lslot << 11) |
                         (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    // 0x80000000 - 31 - enable bit
    // & 0xfc - 1:0 bits of offset always zero

    /* write out the address */
    outdw(PCI_CONFIG_ADDRESS, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    ret = (uint16_t)((indw(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
    return ret;
}

uint16_t pci_check_vendor(uint8_t bus, uint8_t slot) {
    uint16_t vendor, device, cl, prg;
    uint8_t prog_if, rev_id, class_code, subclass;
    /* try and read the first configuration register. Since there are no */
    /* vendors that == 0xFFFF, it must be a non-existent device. */
    if ((vendor = pci_config_read_word(bus, slot, 0, 0)) != 0xFFFF) {
        device = pci_config_read_word(bus, slot, 0, 2);

        prg = pci_config_read_word(bus, slot, 0, 8);
        rev_id = prg & 0xFF;
        prog_if = (prg >> 8);

        cl = pci_config_read_word(bus, slot, 0, 10);
        subclass = cl & 0xFF;
        class_code = (cl >> 8);

        klog("Found PCI device vendor:device=%x:%x \n    rev:prog=%x:%x \n    cl:sc=%s(%x):%x \n    in bus:slot=%x:%x\n",
             vendor, device,
             rev_id, prog_if,
             pci_names[class_code], class_code, subclass,
             bus, slot);
        //klog("Found PCI device v:d=%x:%x, r:p=%x:%x, cl:sc=%x:%x in b:s=%x:%x\n", vendor, device, rev_id, prog_if, class_code, subclass, bus, slot);
    }
    return vendor;
}
