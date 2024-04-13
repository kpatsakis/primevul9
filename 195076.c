static int rtl8139_config_writable(RTL8139State *s)
{
    if ((s->Cfg9346 & Chip9346_op_mask) == Cfg9346_ConfigWrite)
    {
        return 1;
    }

    DPRINTF("Configuration registers are write-protected\n");

    return 0;
}