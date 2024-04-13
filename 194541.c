static void qxl_register_types(void)
{
    type_register_static(&qxl_pci_type_info);
    type_register_static(&qxl_primary_info);
    type_register_static(&qxl_secondary_info);
}