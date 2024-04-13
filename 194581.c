static bool qxl_monitors_config_needed(void *opaque)
{
    PCIQXLDevice *qxl = opaque;

    return qxl->guest_monitors_config != 0;
}