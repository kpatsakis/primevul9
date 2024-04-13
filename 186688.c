static void mptsas_reset(DeviceState *dev)
{
    MPTSASState *s = MPT_SAS(dev);

    mptsas_hard_reset(s);
}