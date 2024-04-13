virNodeDevCapsDefFree(virNodeDevCapsDefPtr caps)
{
    size_t i = 0;
    virNodeDevCapDataPtr data = &caps->data;

    switch (caps->data.type) {
    case VIR_NODE_DEV_CAP_SYSTEM:
        VIR_FREE(data->system.product_name);
        VIR_FREE(data->system.hardware.vendor_name);
        VIR_FREE(data->system.hardware.version);
        VIR_FREE(data->system.hardware.serial);
        VIR_FREE(data->system.firmware.vendor_name);
        VIR_FREE(data->system.firmware.version);
        VIR_FREE(data->system.firmware.release_date);
        break;
    case VIR_NODE_DEV_CAP_PCI_DEV:
        VIR_FREE(data->pci_dev.product_name);
        VIR_FREE(data->pci_dev.vendor_name);
        VIR_FREE(data->pci_dev.physical_function);
        for (i = 0; i < data->pci_dev.num_virtual_functions; i++)
            VIR_FREE(data->pci_dev.virtual_functions[i]);
        VIR_FREE(data->pci_dev.virtual_functions);
        for (i = 0; i < data->pci_dev.nIommuGroupDevices; i++)
            VIR_FREE(data->pci_dev.iommuGroupDevices[i]);
        VIR_FREE(data->pci_dev.iommuGroupDevices);
        virPCIEDeviceInfoFree(data->pci_dev.pci_express);
        for (i = 0; i < data->pci_dev.nmdev_types; i++)
            virMediatedDeviceTypeFree(data->pci_dev.mdev_types[i]);
        VIR_FREE(data->pci_dev.mdev_types);
        break;
    case VIR_NODE_DEV_CAP_USB_DEV:
        VIR_FREE(data->usb_dev.product_name);
        VIR_FREE(data->usb_dev.vendor_name);
        break;
    case VIR_NODE_DEV_CAP_USB_INTERFACE:
        VIR_FREE(data->usb_if.description);
        break;
    case VIR_NODE_DEV_CAP_NET:
        VIR_FREE(data->net.ifname);
        VIR_FREE(data->net.address);
        virBitmapFree(data->net.features);
        data->net.features = NULL;
        break;
    case VIR_NODE_DEV_CAP_SCSI_HOST:
        VIR_FREE(data->scsi_host.wwnn);
        VIR_FREE(data->scsi_host.wwpn);
        VIR_FREE(data->scsi_host.fabric_wwn);
        break;
    case VIR_NODE_DEV_CAP_SCSI_TARGET:
        VIR_FREE(data->scsi_target.name);
        VIR_FREE(data->scsi_target.rport);
        VIR_FREE(data->scsi_target.wwpn);
        break;
    case VIR_NODE_DEV_CAP_SCSI:
        VIR_FREE(data->scsi.type);
        break;
    case VIR_NODE_DEV_CAP_STORAGE:
        VIR_FREE(data->storage.block);
        VIR_FREE(data->storage.bus);
        VIR_FREE(data->storage.drive_type);
        VIR_FREE(data->storage.model);
        VIR_FREE(data->storage.vendor);
        VIR_FREE(data->storage.serial);
        VIR_FREE(data->storage.media_label);
        break;
    case VIR_NODE_DEV_CAP_SCSI_GENERIC:
        VIR_FREE(data->sg.path);
        break;
    case VIR_NODE_DEV_CAP_MDEV:
        VIR_FREE(data->mdev.type);
        VIR_FREE(data->mdev.uuid);
        for (i = 0; i < data->mdev.nattributes; i++)
            virMediatedDeviceAttrFree(data->mdev.attributes[i]);
        VIR_FREE(data->mdev.attributes);
        break;
    case VIR_NODE_DEV_CAP_CSS_DEV:
        for (i = 0; i < data->ccw_dev.nmdev_types; i++)
            virMediatedDeviceTypeFree(data->ccw_dev.mdev_types[i]);
        VIR_FREE(data->ccw_dev.mdev_types);
        break;
    case VIR_NODE_DEV_CAP_MDEV_TYPES:
    case VIR_NODE_DEV_CAP_DRM:
    case VIR_NODE_DEV_CAP_FC_HOST:
    case VIR_NODE_DEV_CAP_VPORTS:
    case VIR_NODE_DEV_CAP_CCW_DEV:
    case VIR_NODE_DEV_CAP_VDPA:
    case VIR_NODE_DEV_CAP_LAST:
        /* This case is here to shutup the compiler */
        break;
    }

    VIR_FREE(caps);
}