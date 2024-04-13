cirrus_hook_read_cr(CirrusVGAState * s, unsigned reg_index, int *reg_value)
{
    switch (reg_index) {
    case 0x00:			// Standard VGA
    case 0x01:			// Standard VGA
    case 0x02:			// Standard VGA
    case 0x03:			// Standard VGA
    case 0x04:			// Standard VGA
    case 0x05:			// Standard VGA
    case 0x06:			// Standard VGA
    case 0x07:			// Standard VGA
    case 0x08:			// Standard VGA
    case 0x09:			// Standard VGA
    case 0x0a:			// Standard VGA
    case 0x0b:			// Standard VGA
    case 0x0c:			// Standard VGA
    case 0x0d:			// Standard VGA
    case 0x0e:			// Standard VGA
    case 0x0f:			// Standard VGA
    case 0x10:			// Standard VGA
    case 0x11:			// Standard VGA
    case 0x12:			// Standard VGA
    case 0x13:			// Standard VGA
    case 0x14:			// Standard VGA
    case 0x15:			// Standard VGA
    case 0x16:			// Standard VGA
    case 0x17:			// Standard VGA
    case 0x18:			// Standard VGA
	return CIRRUS_HOOK_NOT_HANDLED;
    case 0x24:			// Attribute Controller Toggle Readback (R)
        *reg_value = (s->ar_flip_flop << 7);
        break;
    case 0x19:			// Interlace End
    case 0x1a:			// Miscellaneous Control
    case 0x1b:			// Extended Display Control
    case 0x1c:			// Sync Adjust and Genlock
    case 0x1d:			// Overlay Extended Control
    case 0x22:			// Graphics Data Latches Readback (R)
    case 0x25:			// Part Status
    case 0x27:			// Part ID (R)
	*reg_value = s->cr[reg_index];
	break;
    case 0x26:			// Attribute Controller Index Readback (R)
	*reg_value = s->ar_index & 0x3f;
	break;
    default:
#ifdef DEBUG_CIRRUS
	printf("cirrus: inport cr_index %02x\n", reg_index);
	*reg_value = 0xff;
#endif
	break;
    }

    return CIRRUS_HOOK_HANDLED;
}