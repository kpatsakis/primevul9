static void prom9346_decode_command(EEprom9346 *eeprom, uint8_t command)
{
    DPRINTF("eeprom command 0x%02x\n", command);

    switch (command & Chip9346_op_mask)
    {
        case Chip9346_op_read:
        {
            eeprom->address = command & EEPROM_9346_ADDR_MASK;
            eeprom->output = eeprom->contents[eeprom->address];
            eeprom->eedo = 0;
            eeprom->tick = 0;
            eeprom->mode = Chip9346_data_read;
            DPRINTF("eeprom read from address 0x%02x data=0x%04x\n",
                eeprom->address, eeprom->output);
        }
        break;

        case Chip9346_op_write:
        {
            eeprom->address = command & EEPROM_9346_ADDR_MASK;
            eeprom->input = 0;
            eeprom->tick = 0;
            eeprom->mode = Chip9346_none; /* Chip9346_data_write */
            DPRINTF("eeprom begin write to address 0x%02x\n",
                eeprom->address);
        }
        break;
        default:
            eeprom->mode = Chip9346_none;
            switch (command & Chip9346_op_ext_mask)
            {
                case Chip9346_op_write_enable:
                    DPRINTF("eeprom write enabled\n");
                    break;
                case Chip9346_op_write_all:
                    DPRINTF("eeprom begin write all\n");
                    break;
                case Chip9346_op_write_disable:
                    DPRINTF("eeprom write disabled\n");
                    break;
            }
            break;
    }
}