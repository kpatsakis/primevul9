virNodeDeviceCapPCIDefFormat(virBufferPtr buf,
                             const virNodeDevCapData *data)
{
    size_t i;

    if (data->pci_dev.klass >= 0)
        virBufferAsprintf(buf, "<class>0x%.6x</class>\n", data->pci_dev.klass);
    virBufferAsprintf(buf, "<domain>%d</domain>\n",
                      data->pci_dev.domain);
    virBufferAsprintf(buf, "<bus>%d</bus>\n", data->pci_dev.bus);
    virBufferAsprintf(buf, "<slot>%d</slot>\n",
                      data->pci_dev.slot);
    virBufferAsprintf(buf, "<function>%d</function>\n",
                      data->pci_dev.function);
    virBufferAsprintf(buf, "<product id='0x%04x'",
                      data->pci_dev.product);
    if (data->pci_dev.product_name)
        virBufferEscapeString(buf, ">%s</product>\n",
                              data->pci_dev.product_name);
    else
        virBufferAddLit(buf, "/>\n");
    virBufferAsprintf(buf, "<vendor id='0x%04x'",
                      data->pci_dev.vendor);
    if (data->pci_dev.vendor_name)
        virBufferEscapeString(buf, ">%s</vendor>\n",
                              data->pci_dev.vendor_name);
    else
        virBufferAddLit(buf, "/>\n");
    if (data->pci_dev.flags & VIR_NODE_DEV_CAP_FLAG_PCI_PHYSICAL_FUNCTION) {
        virBufferAddLit(buf, "<capability type='phys_function'>\n");
        virBufferAdjustIndent(buf, 2);
        virBufferAsprintf(buf,
                          "<address domain='0x%04x' bus='0x%02x' "
                          "slot='0x%02x' function='0x%d'/>\n",
                          data->pci_dev.physical_function->domain,
                          data->pci_dev.physical_function->bus,
                          data->pci_dev.physical_function->slot,
                          data->pci_dev.physical_function->function);
        virBufferAdjustIndent(buf, -2);
        virBufferAddLit(buf, "</capability>\n");
    }
    if (data->pci_dev.flags & VIR_NODE_DEV_CAP_FLAG_PCI_VIRTUAL_FUNCTION) {
        virBufferAddLit(buf, "<capability type='virt_functions'");
        if (data->pci_dev.max_virtual_functions)
            virBufferAsprintf(buf, " maxCount='%u'",
                              data->pci_dev.max_virtual_functions);
        if (data->pci_dev.num_virtual_functions == 0) {
            virBufferAddLit(buf, "/>\n");
        } else {
            virBufferAddLit(buf, ">\n");
            virBufferAdjustIndent(buf, 2);
            for (i = 0; i < data->pci_dev.num_virtual_functions; i++) {
                virBufferAsprintf(buf,
                                  "<address domain='0x%04x' bus='0x%02x' "
                                  "slot='0x%02x' function='0x%d'/>\n",
                                  data->pci_dev.virtual_functions[i]->domain,
                                  data->pci_dev.virtual_functions[i]->bus,
                                  data->pci_dev.virtual_functions[i]->slot,
                                  data->pci_dev.virtual_functions[i]->function);
            }
            virBufferAdjustIndent(buf, -2);
            virBufferAddLit(buf, "</capability>\n");
        }
    }
    if (data->pci_dev.hdrType) {
        virBufferAsprintf(buf, "<capability type='%s'/>\n",
                          virPCIHeaderTypeToString(data->pci_dev.hdrType));
    }
    if (data->pci_dev.flags & VIR_NODE_DEV_CAP_FLAG_PCI_MDEV) {
        virNodeDeviceCapMdevTypesFormat(buf,
                                        data->pci_dev.mdev_types,
                                        data->pci_dev.nmdev_types);
    }
    if (data->pci_dev.nIommuGroupDevices) {
        virBufferAsprintf(buf, "<iommuGroup number='%d'>\n",
                          data->pci_dev.iommuGroupNumber);
        virBufferAdjustIndent(buf, 2);
        for (i = 0; i < data->pci_dev.nIommuGroupDevices; i++) {
            virBufferAsprintf(buf,
                              "<address domain='0x%04x' bus='0x%02x' "
                              "slot='0x%02x' function='0x%d'/>\n",
                              data->pci_dev.iommuGroupDevices[i]->domain,
                              data->pci_dev.iommuGroupDevices[i]->bus,
                              data->pci_dev.iommuGroupDevices[i]->slot,
                              data->pci_dev.iommuGroupDevices[i]->function);
        }
        virBufferAdjustIndent(buf, -2);
        virBufferAddLit(buf, "</iommuGroup>\n");
    }
    if (data->pci_dev.numa_node >= 0)
        virBufferAsprintf(buf, "<numa node='%d'/>\n",
                          data->pci_dev.numa_node);

    if (data->pci_dev.flags & VIR_NODE_DEV_CAP_FLAG_PCIE)
        virPCIEDeviceInfoFormat(buf, data->pci_dev.pci_express);
}