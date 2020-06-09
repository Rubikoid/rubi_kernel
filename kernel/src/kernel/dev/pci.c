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

static const char *pci_sc_mass_stor_names[PCI_SC_MASS_STOR_COUNT] = {
    "SCSI_STORAGE_CONTROLLER",
    "IDE_INTERFACE",
    "FLOPPY_DISK_CONTROLLER",
    "IPI_BUS_CONTROLLER",
    "RAID_BUS_CONTROLLER",
    "ATA_CONTROLLER",
    "SATA_CONTROLLER",
    "SERIAL_ATTACHED_SCSI_CONTROLLER",
    "NON-VOLATILE_MEMORY_CONTROLLER",
};

static const char *pci_sc_netwrok_names[PCI_SC_NETWROK_COUNT] = {
    "ETHERNET_CONTROLLER",
    "TOKEN_RING_NETWORK_CONTROLLER",
    "FDDI_NETWORK_CONTROLLER",
    "ATM_NETWORK_CONTROLLER",
    "ISDN_CONTROLLER",
    "WORLDFIP_CONTROLLER",
    "PICMG_CONTROLLER",
    "INFINIBAND_CONTROLLER",
    "FABRIC_CONTROLLER",
};

static const char *pci_sc_disp_names[PCI_SC_DISP_COUNT] = {
    "VGA_COMPATIBLE_CONTROLLER",
    "XGA_COMPATIBLE_CONTROLLER",
    "3D_CONTROLLER",
};

static const char *pci_sc_bridge_names[PCI_SC_BRIDGE_COUNT] = {
    "HOST_BRIDGE",
    "ISA_BRIDGE",
    "EISA_BRIDGE",
    "MICROCHANNEL_BRIDGE",
    "PCI_BRIDGE",
    "PCMCIA_BRIDGE",
    "NUBUS_BRIDGE",
    "CARDBUS_BRIDGE",
    "RACEWAY_BRIDGE",
    "SEMI-TRANSPARENT_PCI-TO-PCI_BRIDGE",
    "INFINIBAND_TO_PCI_HOST_BRIDGE",
};

static const char *pci_sc_base_pereph_names[PCI_SC_BASE_PEREPH_COUNT] = {
    "PIC",
    "DMA_CONTROLLER",
    "TIMER",
    "RTC",
    "PCI_HOT-PLUG_CONTROLLER",
    "SD_HOST_CONTROLLER",
    "IOMMU",
};

static const char *pci_sc_serial_bus_names[PCI_SC_SERIAL_BUS_COUNT] = {
    "FIREWIRE",
    "ACCESS_BUS",
    "SSA",
    "USB_CONTROLLER",
    "FIBRE_CHANNEL",
    "SMBUS",
    "INFINIBAND",
    "IPMI_INTERFACE",
    "SERCOS_INTERFACE",
    "CANBUS",
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

uint32_t pci_config_read(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg) {
    uint32_t offset = reg % 0x04;
    reg &= ~0x3;
    outdw(PCI_CONFIG_ADDRESS,
          0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((func & 0x07) << 8) | ((reg & 0xFC)));
    return indw(PCI_CONFIG_DATA) >> (8 * offset);
}

uint32_t inline pci_config_read_dword(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg) {
    return pci_config_read(bus, device, func, reg);
}
uint16_t inline pci_config_read_word(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg) {
    return pci_config_read(bus, device, func, reg) & 0xffff;
}
uint8_t inline pci_config_read_byte(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg) {
    return pci_config_read(bus, device, func, reg) & 0xff;
}

void pci_config_write_dword(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg, uint32_t value) {
    reg &= ~0x3;
    outdw(PCI_CONFIG_ADDRESS,
          0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((func & 0x07) << 8) | ((reg & 0xFC)));
    outdw(PCI_CONFIG_DATA, value);
}

uint16_t pci_check_vendor(uint8_t bus, uint8_t slot) {
    // try and read the first configuration register. Since there are no
    // vendors that == 0xFFFF, it must be a non-existent device.
    struct pci_dev_t dev = {0};
    pci_fill_dev(bus, slot, &dev);
    if (dev.vendor != 0xFFFF) {
        log_device(&dev);
        //klog("Found PCI device v:d=%x:%x, r:p=%x:%x, cl:sc=%x:%x in b:s=%x:%x\n", vendor, device, rev_id, prog_if, class_code, subclass, bus, slot);
    }
    return dev.vendor;
}

void *alloc_pci_mem(struct base_addres_register_t *bar) {
    if (bar->address.raw_value != 0 && bar->address.memory_map.memory_io_type == 0) {
        
    }
    return NULL;
}

void log_device(struct pci_dev_t *dev) {
    const char *desc;
    switch (dev->dev_class.s.class_code) {
        case PCI_CLASS_MASS_STOR: {
            desc = pci_sc_mass_stor_names[dev->dev_class.s.sub_class];
            break;
        }
        case PCI_CLASS_NETWORK: {
            desc = pci_sc_netwrok_names[dev->dev_class.s.sub_class];
            break;
        }
        case PCI_CLASS_DISP: {
            desc = pci_sc_disp_names[dev->dev_class.s.sub_class];
            break;
        }
        case PCI_CLASS_BRIDGE: {
            desc = pci_sc_bridge_names[dev->dev_class.s.sub_class];
            break;
        }
        case PCI_CLASS_BASE_PEREPH: {
            desc = pci_sc_base_pereph_names[dev->dev_class.s.sub_class];
            break;
        }
        case PCI_CLASS_SERIAL_BUS: {
            desc = pci_sc_serial_bus_names[dev->dev_class.s.sub_class];
            break;
        }
        default:
            desc = "NONE";
            break;
    }
    klog("Found PCI device vendor:device=%x:%x\n    ", dev->vendor, dev->device);
    printf("rev:prog=%x:%x\n    ", dev->revision.s.revision_ID, dev->revision.s.prog_IF);
    printf("cl:sc=%s(%x):%s(%x)\n    ", pci_names[dev->dev_class.s.class_code], dev->dev_class.s.class_code, desc, dev->dev_class.s.sub_class);
    if (dev->base_addrs[0].address.memory_map.memory_io_type == 0) {  // MM-IO
        printf("addr[0]=MM, size=%x, off=%x, type=%x\n    ", dev->base_addrs[0].size, dev->base_addrs[0].address.memory_map.offset, dev->base_addrs[0].address.memory_map.type);
    } else if (dev->base_addrs[0].address.memory_map.memory_io_type == 1) {  // PM-IO
        printf("addr[0]=PM, size=%x, off=%x\n    ", dev->base_addrs[0].size, dev->base_addrs[0].address.port_map.offset);
    }
    // printf("addr[0]:addr[1]=%x(%x):%x(%x)\n    ");
    printf("in bus:slot=%x:%x\n", dev->bus, dev->slot);
}

uint16_t pci_fill_dev(uint8_t bus, uint8_t slot, struct pci_dev_t *dev) {
    dev->bus = 0;
    dev->slot = 0;
    if ((dev->vendor = pci_config_read_word(bus, slot, 0, 0 * 2)) != 0xFFFF) {
        dev->bus = bus;
        dev->slot = slot;

        dev->device = pci_config_read_word(bus, slot, 0, 1 * 2);

        dev->command_reg = pci_config_read_word(bus, slot, 0, 2 * 2);
        dev->status_reg = pci_config_read_word(bus, slot, 0, 3 * 2);

        dev->revision.revision = pci_config_read_word(bus, slot, 0, 4 * 2);
        dev->dev_class.dev_class = pci_config_read_word(bus, slot, 0, 5 * 2);

        dev->cache_latency.cache_latency = pci_config_read_word(bus, slot, 0, 6 * 2);
        dev->header_BIST.header_BIST = pci_config_read_word(bus, slot, 0, 7 * 2);

        for (int i = 0; i < 6; i++) {
            int base_addr = 8 * 2 + i * 4;
            dev->base_addrs[i].address.raw_value = pci_config_read_dword(bus, slot, 0, base_addr);
            if (dev->base_addrs[i].address.raw_value != 0) {
                pci_config_write_dword(bus, slot, 0, base_addr, 0xFFFFFFFF);  // get size of mapping data
                dev->base_addrs[i].size = pci_config_read_dword(bus, slot, 0, base_addr);
                dev->base_addrs[i].size = dev->base_addrs[i].address.memory_map.memory_io_type ? dev->base_addrs[i].size & 0xFFFFFFFC : dev->base_addrs[i].size & 0xFFFFFFF0;
                dev->base_addrs[i].size = (~dev->base_addrs[i].size) + 1;

                pci_config_write_dword(bus, slot, 0, base_addr, dev->base_addrs[i].address.raw_value);  // restore orig value
            }
        }

        dev->cardbus_cis_pointer = pci_config_read_dword(bus, slot, 0, 8 * 2 + 6 * 4);

        dev->subsystem_vendor = pci_config_read_word(bus, slot, 0, 8 * 2 + 7 * 4 + 0 * 2);
        dev->subsystem_id = pci_config_read_word(bus, slot, 0, 8 * 2 + 7 * 4 + 1 * 2);

        dev->expansion_rom_base_addr = pci_config_read_dword(bus, slot, 0, 8 * 2 + 7 * 4 + 2 * 2 + 0 * 4);

        dev->interrupts.interrupts = pci_config_read_word(bus, slot, 0, 0x3C);
        dev->grant_latency.grant_latency = pci_config_read_word(bus, slot, 0, 0x3C + 2);
    }
    return dev->vendor;
}
