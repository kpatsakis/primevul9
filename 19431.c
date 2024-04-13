virNodeDeviceGetCSSDynamicCaps(const char *sysfsPath,
                               virNodeDevCapCCWPtr ccw_dev)
{
    ccw_dev->flags &= ~VIR_NODE_DEV_CAP_FLAG_CSS_MDEV;
    if (virNodeDeviceGetMdevTypesCaps(sysfsPath,
                                      &ccw_dev->mdev_types,
                                      &ccw_dev->nmdev_types) < 0)
        return -1;
    if (ccw_dev->nmdev_types > 0)
        ccw_dev->flags |= VIR_NODE_DEV_CAP_FLAG_CSS_MDEV;

    return 0;
}