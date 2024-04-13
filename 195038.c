static void prom9346_shift_clock(EEprom9346 *eeprom)
{
    int bit = eeprom->eedi?1:0;

    ++ eeprom->tick;

    DPRINTF("eeprom: tick %d eedi=%d eedo=%d\n", eeprom->tick, eeprom->eedi,
        eeprom->eedo);

    switch (eeprom->mode)
    {
        case Chip9346_enter_command_mode:
            if (bit)
            {
                eeprom->mode = Chip9346_read_command;
                eeprom->tick = 0;
                eeprom->input = 0;
                DPRINTF("eeprom: +++ synchronized, begin command read\n");
            }
            break;

        case Chip9346_read_command:
            eeprom->input = (eeprom->input << 1) | (bit & 1);
            if (eeprom->tick == 8)
            {
                prom9346_decode_command(eeprom, eeprom->input & 0xff);
            }
            break;

        case Chip9346_data_read:
            eeprom->eedo = (eeprom->output & 0x8000)?1:0;
            eeprom->output <<= 1;
            if (eeprom->tick == 16)
            {
#if 1
        // the FreeBSD drivers (rl and re) don't explicitly toggle
        // CS between reads (or does setting Cfg9346 to 0 count too?),
        // so we need to enter wait-for-command state here
                eeprom->mode = Chip9346_enter_command_mode;
                eeprom->input = 0;
                eeprom->tick = 0;

                DPRINTF("eeprom: +++ end of read, awaiting next command\n");
#else
        // original behaviour
                ++eeprom->address;
                eeprom->address &= EEPROM_9346_ADDR_MASK;
                eeprom->output = eeprom->contents[eeprom->address];
                eeprom->tick = 0;

                DPRINTF("eeprom: +++ read next address 0x%02x data=0x%04x\n",
                    eeprom->address, eeprom->output);
#endif
            }
            break;

        case Chip9346_data_write:
            eeprom->input = (eeprom->input << 1) | (bit & 1);
            if (eeprom->tick == 16)
            {
                DPRINTF("eeprom write to address 0x%02x data=0x%04x\n",
                    eeprom->address, eeprom->input);

                eeprom->contents[eeprom->address] = eeprom->input;
                eeprom->mode = Chip9346_none; /* waiting for next command after CS cycle */
                eeprom->tick = 0;
                eeprom->input = 0;
            }
            break;

        case Chip9346_data_write_all:
            eeprom->input = (eeprom->input << 1) | (bit & 1);
            if (eeprom->tick == 16)
            {
                int i;
                for (i = 0; i < EEPROM_9346_SIZE; i++)
                {
                    eeprom->contents[i] = eeprom->input;
                }
                DPRINTF("eeprom filled with data=0x%04x\n", eeprom->input);

                eeprom->mode = Chip9346_enter_command_mode;
                eeprom->tick = 0;
                eeprom->input = 0;
            }
            break;

        default:
            break;
    }
}