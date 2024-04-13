virNodeDeviceDefFormat(const virNodeDeviceDef *def)
{
    g_auto(virBuffer) buf = VIR_BUFFER_INITIALIZER;
    virNodeDevCapsDefPtr caps;
    size_t i = 0;

    virBufferAddLit(&buf, "<device>\n");
    virBufferAdjustIndent(&buf, 2);
    virBufferEscapeString(&buf, "<name>%s</name>\n", def->name);
    virBufferEscapeString(&buf, "<path>%s</path>\n", def->sysfs_path);
    if (def->devnode)
        virBufferEscapeString(&buf, "<devnode type='dev'>%s</devnode>\n",
                              def->devnode);
    if (def->devlinks) {
        for (i = 0; def->devlinks[i]; i++)
            virBufferEscapeString(&buf, "<devnode type='link'>%s</devnode>\n",
                                  def->devlinks[i]);
    }
    if (def->parent)
        virBufferEscapeString(&buf, "<parent>%s</parent>\n", def->parent);
    if (def->driver) {
        virBufferAddLit(&buf, "<driver>\n");
        virBufferAdjustIndent(&buf, 2);
        virBufferEscapeString(&buf, "<name>%s</name>\n", def->driver);
        virBufferAdjustIndent(&buf, -2);
        virBufferAddLit(&buf, "</driver>\n");
    }

    for (caps = def->caps; caps; caps = caps->next) {
        virNodeDevCapDataPtr data = &caps->data;

        virBufferAsprintf(&buf, "<capability type='%s'>\n",
                          virNodeDevCapTypeToString(caps->data.type));
        virBufferAdjustIndent(&buf, 2);
        switch (caps->data.type) {
        case VIR_NODE_DEV_CAP_SYSTEM:
            virNodeDeviceCapSystemDefFormat(&buf, data);
            break;
        case VIR_NODE_DEV_CAP_PCI_DEV:
            virNodeDeviceCapPCIDefFormat(&buf, data);
            break;
        case VIR_NODE_DEV_CAP_USB_DEV:
            virNodeDeviceCapUSBDevDefFormat(&buf, data);
            break;
        case VIR_NODE_DEV_CAP_USB_INTERFACE:
            virNodeDeviceCapUSBInterfaceDefFormat(&buf, data);
            break;
        case VIR_NODE_DEV_CAP_NET:
            virNodeDeviceCapNetDefFormat(&buf, data);
            break;
        case VIR_NODE_DEV_CAP_SCSI_HOST:
            virNodeDeviceCapSCSIHostDefFormat(&buf, data);
            break;
        case VIR_NODE_DEV_CAP_SCSI_TARGET:
            virBufferEscapeString(&buf, "<target>%s</target>\n",
                                  data->scsi_target.name);
            if (data->scsi_target.flags & VIR_NODE_DEV_CAP_FLAG_FC_RPORT) {
                virBufferAddLit(&buf, "<capability type='fc_remote_port'>\n");
                virBufferAdjustIndent(&buf, 2);
                virBufferAsprintf(&buf, "<rport>%s</rport>\n",
                                  data->scsi_target.rport);
                virBufferAsprintf(&buf, "<wwpn>%s</wwpn>\n",
                                  data->scsi_target.wwpn);
                virBufferAdjustIndent(&buf, -2);
                virBufferAddLit(&buf, "</capability>\n");
            }
            break;
        case VIR_NODE_DEV_CAP_SCSI:
            virNodeDeviceCapSCSIDefFormat(&buf, data);
            break;
        case VIR_NODE_DEV_CAP_STORAGE:
            virNodeDeviceCapStorageDefFormat(&buf, data);
            break;
        case VIR_NODE_DEV_CAP_SCSI_GENERIC:
            virBufferEscapeString(&buf, "<char>%s</char>\n",
                                  data->sg.path);
            break;
        case VIR_NODE_DEV_CAP_DRM:
            virBufferEscapeString(&buf, "<type>%s</type>\n", virNodeDevDRMTypeToString(data->drm.type));
            break;
        case VIR_NODE_DEV_CAP_MDEV:
            virNodeDeviceCapMdevDefFormat(&buf, data);
            break;
        case VIR_NODE_DEV_CAP_CCW_DEV:
        case VIR_NODE_DEV_CAP_CSS_DEV:
            virNodeDeviceCapCCWDefFormat(&buf, data);
            break;
        case VIR_NODE_DEV_CAP_VDPA:
            virNodeDeviceCapVDPADefFormat(&buf, data);
            break;
        case VIR_NODE_DEV_CAP_MDEV_TYPES:
        case VIR_NODE_DEV_CAP_FC_HOST:
        case VIR_NODE_DEV_CAP_VPORTS:
        case VIR_NODE_DEV_CAP_LAST:
            break;
        }

        virBufferAdjustIndent(&buf, -2);
        virBufferAddLit(&buf, "</capability>\n");
    }

    virBufferAdjustIndent(&buf, -2);
    virBufferAddLit(&buf, "</device>\n");

    return virBufferContentAndReset(&buf);
}