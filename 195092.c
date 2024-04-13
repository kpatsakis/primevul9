static int rtl8139_post_load(void *opaque, int version_id)
{
    RTL8139State* s = opaque;
    rtl8139_set_next_tctr_time(s);
    if (version_id < 4) {
        s->cplus_enabled = s->CpCmd != 0;
    }

    /* nc.link_down can't be migrated, so infer link_down according
     * to link status bit in BasicModeStatus */
    qemu_get_queue(s->nic)->link_down = (s->BasicModeStatus & 0x04) == 0;

    return 0;
}