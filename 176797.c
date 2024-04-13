static uint32_t vga_ioport_read(void *opaque, uint32_t addr)
{
    CirrusVGAState *s = opaque;
    int val, index;

    /* check port range access depending on color/monochrome mode */
    if ((addr >= 0x3b0 && addr <= 0x3bf && (s->msr & MSR_COLOR_EMULATION))
	|| (addr >= 0x3d0 && addr <= 0x3df
	    && !(s->msr & MSR_COLOR_EMULATION))) {
	val = 0xff;
    } else {
	switch (addr) {
	case 0x3c0:
	    if (s->ar_flip_flop == 0) {
		val = s->ar_index;
	    } else {
		val = 0;
	    }
	    break;
	case 0x3c1:
	    index = s->ar_index & 0x1f;
	    if (index < 21)
		val = s->ar[index];
	    else
		val = 0;
	    break;
	case 0x3c2:
	    val = s->st00;
	    break;
	case 0x3c4:
	    val = s->sr_index;
	    break;
	case 0x3c5:
	    if (cirrus_hook_read_sr(s, s->sr_index, &val))
		break;
	    val = s->sr[s->sr_index];
#ifdef DEBUG_VGA_REG
	    printf("vga: read SR%x = 0x%02x\n", s->sr_index, val);
#endif
	    break;
	case 0x3c6:
	    cirrus_read_hidden_dac(s, &val);
	    break;
	case 0x3c7:
	    val = s->dac_state;
	    break;
	case 0x3c8:
	    val = s->dac_write_index;
	    s->cirrus_hidden_dac_lockindex = 0;
	    break;
        case 0x3c9:
	    if (cirrus_hook_read_palette(s, &val))
		break;
	    val = s->palette[s->dac_read_index * 3 + s->dac_sub_index];
	    if (++s->dac_sub_index == 3) {
		s->dac_sub_index = 0;
		s->dac_read_index++;
	    }
	    break;
	case 0x3ca:
	    val = s->fcr;
	    break;
	case 0x3cc:
	    val = s->msr;
	    break;
	case 0x3ce:
	    val = s->gr_index;
	    break;
	case 0x3cf:
	    if (cirrus_hook_read_gr(s, s->gr_index, &val))
		break;
	    val = s->gr[s->gr_index];
#ifdef DEBUG_VGA_REG
	    printf("vga: read GR%x = 0x%02x\n", s->gr_index, val);
#endif
	    break;
	case 0x3b4:
	case 0x3d4:
	    val = s->cr_index;
	    break;
	case 0x3b5:
	case 0x3d5:
	    if (cirrus_hook_read_cr(s, s->cr_index, &val))
		break;
	    val = s->cr[s->cr_index];
#ifdef DEBUG_VGA_REG
	    printf("vga: read CR%x = 0x%02x\n", s->cr_index, val);
#endif
	    break;
	case 0x3ba:
	case 0x3da:
	    /* just toggle to fool polling */
	    val = s->st01 = s->retrace((VGAState *) s);
	    s->ar_flip_flop = 0;
	    break;
	default:
	    val = 0x00;
	    break;
	}
    }
#if defined(DEBUG_VGA)
    printf("VGA: read addr=0x%04x data=0x%02x\n", addr, val);
#endif
    return val;
}