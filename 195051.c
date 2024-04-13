static void rtl8139_Cfg9346_write(RTL8139State *s, uint32_t val)
{
    DeviceState *d = DEVICE(s);

    val &= 0xff;

    DPRINTF("Cfg9346 write val=0x%02x\n", val);

    /* mask unwritable bits */
    val = SET_MASKED(val, 0x31, s->Cfg9346);

    uint32_t opmode = val & 0xc0;
    uint32_t eeprom_val = val & 0xf;

    if (opmode == 0x80) {
        /* eeprom access */
        int eecs = (eeprom_val & 0x08)?1:0;
        int eesk = (eeprom_val & 0x04)?1:0;
        int eedi = (eeprom_val & 0x02)?1:0;
        prom9346_set_wire(s, eecs, eesk, eedi);
    } else if (opmode == 0x40) {
        /* Reset.  */
        val = 0;
        rtl8139_reset(d);
    }

    s->Cfg9346 = val;
}