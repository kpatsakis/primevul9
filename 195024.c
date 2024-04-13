static uint32_t rtl8139_Cfg9346_read(RTL8139State *s)
{
    uint32_t ret = s->Cfg9346;

    uint32_t opmode = ret & 0xc0;

    if (opmode == 0x80)
    {
        /* eeprom access */
        int eedo = prom9346_get_wire(s);
        if (eedo)
        {
            ret |=  0x01;
        }
        else
        {
            ret &= ~0x01;
        }
    }

    DPRINTF("Cfg9346 read val=0x%02x\n", ret);

    return ret;
}