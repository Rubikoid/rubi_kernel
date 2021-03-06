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

#define PCI_SC_MASS_STOR_SCSI_STORAGE_CONTROLLER         0x00
#define PCI_SC_MASS_STOR_IDE_INTERFACE                   0x01
#define PCI_SC_MASS_STOR_FLOPPY_DISK_CONTROLLER          0x02
#define PCI_SC_MASS_STOR_IPI_BUS_CONTROLLER              0x03
#define PCI_SC_MASS_STOR_RAID_BUS_CONTROLLER             0x04
#define PCI_SC_MASS_STOR_ATA_CONTROLLER                  0x05
#define PCI_SC_MASS_STOR_SATA_CONTROLLER                 0x06
#define PCI_SC_MASS_STOR_SERIAL_ATTACHED_SCSI_CONTROLLER 0x07
#define PCI_SC_MASS_STOR_NONVOLATILE_MEMORY_CONTROLLER   0x08
#define PCI_SC_MASS_STOR_COUNT                           0x09

#define PCI_SC_NETWROK_ETHERNET_CONTROLLER           0x00
#define PCI_SC_NETWROK_TOKEN_RING_NETWORK_CONTROLLER 0x01
#define PCI_SC_NETWROK_FDDI_NETWORK_CONTROLLER       0x02
#define PCI_SC_NETWROK_ATM_NETWORK_CONTROLLER        0x03
#define PCI_SC_NETWROK_ISDN_CONTROLLER               0x04
#define PCI_SC_NETWROK_WORLDFIP_CONTROLLER           0x05
#define PCI_SC_NETWROK_PICMG_CONTROLLER              0x06
#define PCI_SC_NETWROK_INFINIBAND_CONTROLLER         0x07
#define PCI_SC_NETWROK_FABRIC_CONTROLLER             0x08
#define PCI_SC_NETWROK_COUNT                         0x09

#define PCI_SC_DISP_VGA_COMPATIBLE_CONTROLLER 0x00
#define PCI_SC_DISP_XGA_COMPATIBLE_CONTROLLER 0x01
#define PCI_SC_DISP_3D_CONTROLLER             0x02
#define PCI_SC_DISP_COUNT                     0x03

#define PCI_SC_BRIDGE_HOST_BRIDGE                        0x00
#define PCI_SC_BRIDGE_ISA_BRIDGE                         0x01
#define PCI_SC_BRIDGE_EISA_BRIDGE                        0x02
#define PCI_SC_BRIDGE_MICROCHANNEL_BRIDGE                0x03
#define PCI_SC_BRIDGE_PCI_BRIDGE                         0x04
#define PCI_SC_BRIDGE_PCMCIA_BRIDGE                      0x05
#define PCI_SC_BRIDGE_NUBUS_BRIDGE                       0x06
#define PCI_SC_BRIDGE_CARDBUS_BRIDGE                     0x07
#define PCI_SC_BRIDGE_RACEWAY_BRIDGE                     0x08
#define PCI_SC_BRIDGE_SEMI_TRANSPARENT_PCI_TO_PCI_BRIDGE 0x09
#define PCI_SC_BRIDGE_INFINIBAND_TO_PCI_HOST_BRIDGE      0x0A
#define PCI_SC_BRIDGE_COUNT                              0x0B

#define PCI_SC_BASE_PEREPH_PIC                     0x00
#define PCI_SC_BASE_PEREPH_DMA_CONTROLLER          0x01
#define PCI_SC_BASE_PEREPH_TIMER                   0x02
#define PCI_SC_BASE_PEREPH_RTC                     0x03
#define PCI_SC_BASE_PEREPH_PCI_HOT_PLUG_CONTROLLER 0x04
#define PCI_SC_BASE_PEREPH_SD_HOST_CONTROLLER      0x05
#define PCI_SC_BASE_PEREPH_IOMMU                   0x06
#define PCI_SC_BASE_PEREPH_COUNT                   0x07

#define PCI_SC_SERIAL_BUS_FIREWIRE         0x00
#define PCI_SC_SERIAL_BUS_ACCESS_BUS       0x01
#define PCI_SC_SERIAL_BUS_SSA              0x02
#define PCI_SC_SERIAL_BUS_USB_CONTROLLER   0x03
#define PCI_SC_SERIAL_BUS_FIBRE_CHANNEL    0x04
#define PCI_SC_SERIAL_BUS_SMBUS            0x05
#define PCI_SC_SERIAL_BUS_INFINIBAND       0x06
#define PCI_SC_SERIAL_BUS_IPMI_INTERFACE   0x07
#define PCI_SC_SERIAL_BUS_SERCOS_INTERFACE 0x08
#define PCI_SC_SERIAL_BUS_CANBUS           0x09
#define PCI_SC_SERIAL_BUS_COUNT            0x0A

#define PCI_DRIVERS_COUNT 1
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

struct base_addres_register_t {
    uint32_t size;  // size of memory area
    union {
        uint32_t raw_value;  // raw, unparsed value.
        struct {
            unsigned int memory_io_type : 1;
            unsigned int type : 2;
            unsigned int prefetchable : 1;
            unsigned int offset : 28;
        } memory_map;
        struct {
            unsigned int memory_io_type : 1;
            unsigned int reserved : 1;
            unsigned int offset : 30;
        } port_map;
    } address;  // address, readen from port

    uint32_t tables_count;
    struct page_table_entry_t **tables;
    void *base_linear_addr;
};

struct pci_dev_t {
    uint8_t bus;
    uint8_t slot;
    uint8_t func;

    uint16_t vendor;
    uint16_t device;
    uint16_t command_reg;  // commands register
    uint16_t status_reg;   // status register

    union {
        uint16_t revision;
        struct {
            uint8_t revision_ID;
            uint8_t prog_IF;
        } s;
    } revision;

    union {
        uint16_t dev_class;
        struct {
            uint8_t sub_class;
            uint8_t class_code;
        } s;
    } dev_class;

    union {
        uint16_t cache_latency;
        struct {
            uint8_t cache_line_size;
            uint8_t latency_timer;
        } s;
    } cache_latency;

    union {
        uint16_t header_BIST;
        struct {
            uint8_t header_type;
            uint8_t BIST;
        } s;
    } header_BIST;

    struct base_addres_register_t base_addrs[6];
    uint32_t cardbus_cis_pointer;

    uint16_t subsystem_vendor;
    uint16_t subsystem_id;

    uint32_t expansion_rom_base_addr;

    union {
        uint16_t interrupts;
        struct {
            uint8_t interrupt_line;
            uint8_t interrupt_pin;
        } s;
    } interrupts;

    union {
        uint16_t grant_latency;
        struct {
            uint8_t min_grant;
            uint8_t max_latency;
        } s;
    } grant_latency;
};

typedef void (*pci_driver_init_fn_t)(struct pci_dev_t *dev);

struct pci_driver_dev_t {
    struct clist_head_t list_head;

    uint16_t vendor_mask;
    uint16_t device_mask;

    union {
        uint16_t dev_class_mask;
        struct {
            uint8_t sub_class_mask;
            uint8_t class_code_mask;
        } s;
    } dev_class_mask;

    union {
        uint16_t revision_mask;
        struct {
            uint8_t revision_ID_mask;
            uint8_t prog_IF_mask;
        } s;
    } revision_mask;

    pci_driver_init_fn_t init;
};

typedef void (*pci_driver_register_fn_t)(struct pci_driver_dev_t* drv_dev);

extern pci_driver_register_fn_t pci_driver_register_list[PCI_DRIVERS_COUNT];
extern struct clist_def_t pci_drivers_def;

extern const char *pci_dev_name;

extern void pci_init();
extern void pci_ih_low(uint32_t number, struct ih_low_data_t *data);
extern void pci_ih_high(struct message_t *msg);

extern void pci_ioctl(uint32_t command, uint32_t subcm);

extern uint32_t pci_write(void *buf, uint32_t *offset, uint32_t size);
// extern void pci_write_ch(char ch);

extern uint32_t pci_read(void *buf, uint32_t *offset, uint32_t size);
//extern char pci_read_ch();

uint32_t pci_config_read(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg);
uint32_t pci_config_read_dword(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg);
uint16_t pci_config_read_word(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg);
uint8_t pci_config_read_byte(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg);

void pci_config_write_dword(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg, uint32_t value);

void *alloc_pci_mem(struct base_addres_register_t *bar);
uint16_t pci_fill_dev(uint8_t bus, uint8_t slot, uint8_t function, struct pci_dev_t *dev);
void log_device(struct pci_dev_t *dev);
uint16_t pci_check_vendor(uint8_t bus, uint8_t slot, uint8_t function);

#endif
