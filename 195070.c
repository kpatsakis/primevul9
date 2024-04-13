static bool rtl8139_hotplug_ready_needed(void *opaque)
{
    return qdev_machine_modified();
}