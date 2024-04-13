static void prom9346_set_wire(RTL8139State *s, int eecs, int eesk, int eedi)
{
    EEprom9346 *eeprom = &s->eeprom;
    uint8_t old_eecs = eeprom->eecs;
    uint8_t old_eesk = eeprom->eesk;

    eeprom->eecs = eecs;
    eeprom->eesk = eesk;
    eeprom->eedi = eedi;

    DPRINTF("eeprom: +++ wires CS=%d SK=%d DI=%d DO=%d\n", eeprom->eecs,
        eeprom->eesk, eeprom->eedi, eeprom->eedo);

    if (!old_eecs && eecs)
    {
        /* Synchronize start */
        eeprom->tick = 0;
        eeprom->input = 0;
        eeprom->output = 0;
        eeprom->mode = Chip9346_enter_command_mode;

        DPRINTF("=== eeprom: begin access, enter command mode\n");
    }

    if (!eecs)
    {
        DPRINTF("=== eeprom: end access\n");
        return;
    }

    if (!old_eesk && eesk)
    {
        /* SK front rules */
        prom9346_shift_clock(eeprom);
    }
}