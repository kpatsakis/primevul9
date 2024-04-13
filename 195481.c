static void ohci_register_types(void)
{
    type_register_static(&ohci_pci_info);
    type_register_static(&ohci_sysbus_info);
}