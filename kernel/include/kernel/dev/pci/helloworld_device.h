#include <kernel/dev/pci.h>
#include <types.h>

#ifndef KERNEL_DEV_PCI_HELLOWORLD_DEVICE_H_
#define KERNEL_DEV_PCI_HELLOWORLD_DEVICE_H_

void helloword_pci_driver_register(struct pci_driver_dev_t* drv_dev);
void helloword_pci_driver_init(struct pci_dev_t *dev);

#endif