#include <kernel/dev/pci.h>

void helloword_pci_driver_register(struct pci_driver_dev_t* drv_dev) {
    drv_dev->vendor_mask = 0;
    drv_dev->device_mask = 0;

    
}

void helloword_pci_driver_init(struct pci_dev_t *dev) {

}
