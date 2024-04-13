static int prom9346_get_wire(RTL8139State *s)
{
    EEprom9346 *eeprom = &s->eeprom;
    if (!eeprom->eecs)
        return 0;

    return eeprom->eedo;
}