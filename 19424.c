virNodeDeviceCapCCWDefFormat(virBufferPtr buf,
                             const virNodeDevCapData *data)
{
    virBufferAsprintf(buf, "<cssid>0x%x</cssid>\n",
                      data->ccw_dev.cssid);
    virBufferAsprintf(buf, "<ssid>0x%x</ssid>\n",
                      data->ccw_dev.ssid);
    virBufferAsprintf(buf, "<devno>0x%04x</devno>\n",
                      data->ccw_dev.devno);
    if (data->ccw_dev.flags & VIR_NODE_DEV_CAP_FLAG_CSS_MDEV)
        virNodeDeviceCapMdevTypesFormat(buf,
                                        data->ccw_dev.mdev_types,
                                        data->ccw_dev.nmdev_types);
}