static int i740fb_pan_display(struct fb_var_screeninfo *var,
				 struct fb_info *info)
{
	struct i740fb_par *par = info->par;
	u32 base = (var->yoffset * info->var.xres_virtual
		 + (var->xoffset & ~7)) >> 2;

	dev_dbg(info->device, "pan_display: xoffset: %i yoffset: %i base: %i\n",
		var->xoffset, var->yoffset, base);

	switch (info->var.bits_per_pixel) {
	case 8:
		break;
	case 15:
	case 16:
		base *= 2;
		break;
	case 24:
		/*
		 * The last bit does not seem to have any effect on the start
		 * address register in 24bpp mode, so...
		 */
		base &= 0xFFFFFFFE; /* ...ignore the last bit. */
		base *= 3;
		break;
	case 32:
		base *= 4;
		break;
	}

	par->crtc[VGA_CRTC_START_LO] = base & 0x000000FF;
	par->crtc[VGA_CRTC_START_HI] = (base & 0x0000FF00) >>  8;
	par->ext_start_addr_hi = (base & 0x3FC00000) >> 22;
	par->ext_start_addr =
			((base & 0x003F0000) >> 16) | EXT_START_ADDR_ENABLE;

	i740outreg(par, VGA_CRT_IC, VGA_CRTC_START_LO,  base & 0x000000FF);
	i740outreg(par, VGA_CRT_IC, VGA_CRTC_START_HI,
			(base & 0x0000FF00) >> 8);
	i740outreg(par, VGA_CRT_IC, EXT_START_ADDR_HI,
			(base & 0x3FC00000) >> 22);
	i740outreg(par, VGA_CRT_IC, EXT_START_ADDR,
			((base & 0x003F0000) >> 16) | EXT_START_ADDR_ENABLE);

	return 0;
}