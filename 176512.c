static int i740fb_decode_var(const struct fb_var_screeninfo *var,
			     struct i740fb_par *par, struct fb_info *info)
{
	/*
	 * Get the video params out of 'var'.
	 * If a value doesn't fit, round it up, if it's too big, return -EINVAL.
	 */

	u32 xres, right, hslen, left, xtotal;
	u32 yres, lower, vslen, upper, ytotal;
	u32 vxres, xoffset, vyres, yoffset;
	u32 bpp, base, dacspeed24, mem;
	u8 r7;
	int i;

	dev_dbg(info->device, "decode_var: xres: %i, yres: %i, xres_v: %i, xres_v: %i\n",
		  var->xres, var->yres, var->xres_virtual, var->xres_virtual);
	dev_dbg(info->device, "	xoff: %i, yoff: %i, bpp: %i, graysc: %i\n",
		  var->xoffset, var->yoffset, var->bits_per_pixel,
		  var->grayscale);
	dev_dbg(info->device, "	activate: %i, nonstd: %i, vmode: %i\n",
		  var->activate, var->nonstd, var->vmode);
	dev_dbg(info->device, "	pixclock: %i, hsynclen:%i, vsynclen:%i\n",
		  var->pixclock, var->hsync_len, var->vsync_len);
	dev_dbg(info->device, "	left: %i, right: %i, up:%i, lower:%i\n",
		  var->left_margin, var->right_margin, var->upper_margin,
		  var->lower_margin);


	bpp = var->bits_per_pixel;
	switch (bpp) {
	case 1 ... 8:
		bpp = 8;
		if ((1000000 / var->pixclock) > DACSPEED8) {
			dev_err(info->device, "requested pixclock %i MHz out of range (max. %i MHz at 8bpp)\n",
				1000000 / var->pixclock, DACSPEED8);
			return -EINVAL;
		}
		break;
	case 9 ... 15:
		bpp = 15;
		fallthrough;
	case 16:
		if ((1000000 / var->pixclock) > DACSPEED16) {
			dev_err(info->device, "requested pixclock %i MHz out of range (max. %i MHz at 15/16bpp)\n",
				1000000 / var->pixclock, DACSPEED16);
			return -EINVAL;
		}
		break;
	case 17 ... 24:
		bpp = 24;
		dacspeed24 = par->has_sgram ? DACSPEED24_SG : DACSPEED24_SD;
		if ((1000000 / var->pixclock) > dacspeed24) {
			dev_err(info->device, "requested pixclock %i MHz out of range (max. %i MHz at 24bpp)\n",
				1000000 / var->pixclock, dacspeed24);
			return -EINVAL;
		}
		break;
	case 25 ... 32:
		bpp = 32;
		if ((1000000 / var->pixclock) > DACSPEED32) {
			dev_err(info->device, "requested pixclock %i MHz out of range (max. %i MHz at 32bpp)\n",
				1000000 / var->pixclock, DACSPEED32);
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	xres = ALIGN(var->xres, 8);
	vxres = ALIGN(var->xres_virtual, 16);
	if (vxres < xres)
		vxres = xres;

	xoffset = ALIGN(var->xoffset, 8);
	if (xres + xoffset > vxres)
		xoffset = vxres - xres;

	left = ALIGN(var->left_margin, 8);
	right = ALIGN(var->right_margin, 8);
	hslen = ALIGN(var->hsync_len, 8);

	yres = var->yres;
	vyres = var->yres_virtual;
	if (yres > vyres)
		vyres = yres;

	yoffset = var->yoffset;
	if (yres + yoffset > vyres)
		yoffset = vyres - yres;

	lower = var->lower_margin;
	vslen = var->vsync_len;
	upper = var->upper_margin;

	mem = vxres * vyres * ((bpp + 1) / 8);
	if (mem > info->screen_size) {
		dev_err(info->device, "not enough video memory (%d KB requested, %ld KB available)\n",
			mem >> 10, info->screen_size >> 10);
		return -ENOMEM;
	}

	if (yoffset + yres > vyres)
		yoffset = vyres - yres;

	xtotal = xres + right + hslen + left;
	ytotal = yres + lower + vslen + upper;

	par->crtc[VGA_CRTC_H_TOTAL] = (xtotal >> 3) - 5;
	par->crtc[VGA_CRTC_H_DISP] = (xres >> 3) - 1;
	par->crtc[VGA_CRTC_H_BLANK_START] = ((xres + right) >> 3) - 1;
	par->crtc[VGA_CRTC_H_SYNC_START] = (xres + right) >> 3;
	par->crtc[VGA_CRTC_H_SYNC_END] = (((xres + right + hslen) >> 3) & 0x1F)
		| ((((xres + right + hslen) >> 3) & 0x20) << 2);
	par->crtc[VGA_CRTC_H_BLANK_END] = ((xres + right + hslen) >> 3 & 0x1F)
		| 0x80;

	par->crtc[VGA_CRTC_V_TOTAL] = ytotal - 2;

	r7 = 0x10;	/* disable linecompare */
	if (ytotal & 0x100)
		r7 |= 0x01;
	if (ytotal & 0x200)
		r7 |= 0x20;

	par->crtc[VGA_CRTC_PRESET_ROW] = 0;
	par->crtc[VGA_CRTC_MAX_SCAN] = 0x40;	/* 1 scanline, no linecmp */
	if (var->vmode & FB_VMODE_DOUBLE)
		par->crtc[VGA_CRTC_MAX_SCAN] |= 0x80;
	par->crtc[VGA_CRTC_CURSOR_START] = 0x00;
	par->crtc[VGA_CRTC_CURSOR_END] = 0x00;
	par->crtc[VGA_CRTC_CURSOR_HI] = 0x00;
	par->crtc[VGA_CRTC_CURSOR_LO] = 0x00;
	par->crtc[VGA_CRTC_V_DISP_END] = yres-1;
	if ((yres-1) & 0x100)
		r7 |= 0x02;
	if ((yres-1) & 0x200)
		r7 |= 0x40;

	par->crtc[VGA_CRTC_V_BLANK_START] = yres + lower - 1;
	par->crtc[VGA_CRTC_V_SYNC_START] = yres + lower - 1;
	if ((yres + lower - 1) & 0x100)
		r7 |= 0x0C;
	if ((yres + lower - 1) & 0x200) {
		par->crtc[VGA_CRTC_MAX_SCAN] |= 0x20;
		r7 |= 0x80;
	}

	/* disabled IRQ */
	par->crtc[VGA_CRTC_V_SYNC_END] =
		((yres + lower - 1 + vslen) & 0x0F) & ~0x10;
	/* 0x7F for VGA, but some SVGA chips require all 8 bits to be set */
	par->crtc[VGA_CRTC_V_BLANK_END] = (yres + lower - 1 + vslen) & 0xFF;

	par->crtc[VGA_CRTC_UNDERLINE] = 0x00;
	par->crtc[VGA_CRTC_MODE] = 0xC3 ;
	par->crtc[VGA_CRTC_LINE_COMPARE] = 0xFF;
	par->crtc[VGA_CRTC_OVERFLOW] = r7;

	par->vss = 0x00;	/* 3DA */

	for (i = 0x00; i < 0x10; i++)
		par->atc[i] = i;
	par->atc[VGA_ATC_MODE] = 0x81;
	par->atc[VGA_ATC_OVERSCAN] = 0x00;	/* 0 for EGA, 0xFF for VGA */
	par->atc[VGA_ATC_PLANE_ENABLE] = 0x0F;
	par->atc[VGA_ATC_COLOR_PAGE] = 0x00;

	par->misc = 0xC3;
	if (var->sync & FB_SYNC_HOR_HIGH_ACT)
		par->misc &= ~0x40;
	if (var->sync & FB_SYNC_VERT_HIGH_ACT)
		par->misc &= ~0x80;

	par->seq[VGA_SEQ_CLOCK_MODE] = 0x01;
	par->seq[VGA_SEQ_PLANE_WRITE] = 0x0F;
	par->seq[VGA_SEQ_CHARACTER_MAP] = 0x00;
	par->seq[VGA_SEQ_MEMORY_MODE] = 0x06;

	par->gdc[VGA_GFX_SR_VALUE] = 0x00;
	par->gdc[VGA_GFX_SR_ENABLE] = 0x00;
	par->gdc[VGA_GFX_COMPARE_VALUE] = 0x00;
	par->gdc[VGA_GFX_DATA_ROTATE] = 0x00;
	par->gdc[VGA_GFX_PLANE_READ] = 0;
	par->gdc[VGA_GFX_MODE] = 0x02;
	par->gdc[VGA_GFX_MISC] = 0x05;
	par->gdc[VGA_GFX_COMPARE_MASK] = 0x0F;
	par->gdc[VGA_GFX_BIT_MASK] = 0xFF;

	base = (yoffset * vxres + (xoffset & ~7)) >> 2;
	switch (bpp) {
	case 8:
		par->crtc[VGA_CRTC_OFFSET] = vxres >> 3;
		par->ext_offset = vxres >> 11;
		par->pixelpipe_cfg1 = DISPLAY_8BPP_MODE;
		par->bitblt_cntl = COLEXP_8BPP;
		break;
	case 15: /* 0rrrrrgg gggbbbbb */
	case 16: /* rrrrrggg gggbbbbb */
		par->pixelpipe_cfg1 = (var->green.length == 6) ?
			DISPLAY_16BPP_MODE : DISPLAY_15BPP_MODE;
		par->crtc[VGA_CRTC_OFFSET] = vxres >> 2;
		par->ext_offset = vxres >> 10;
		par->bitblt_cntl = COLEXP_16BPP;
		base *= 2;
		break;
	case 24:
		par->crtc[VGA_CRTC_OFFSET] = (vxres * 3) >> 3;
		par->ext_offset = (vxres * 3) >> 11;
		par->pixelpipe_cfg1 = DISPLAY_24BPP_MODE;
		par->bitblt_cntl = COLEXP_24BPP;
		base &= 0xFFFFFFFE; /* ...ignore the last bit. */
		base *= 3;
		break;
	case 32:
		par->crtc[VGA_CRTC_OFFSET] = vxres >> 1;
		par->ext_offset = vxres >> 9;
		par->pixelpipe_cfg1 = DISPLAY_32BPP_MODE;
		par->bitblt_cntl = COLEXP_RESERVED; /* Unimplemented on i740 */
		base *= 4;
		break;
	}

	par->crtc[VGA_CRTC_START_LO] = base & 0x000000FF;
	par->crtc[VGA_CRTC_START_HI] = (base & 0x0000FF00) >>  8;
	par->ext_start_addr =
		((base & 0x003F0000) >> 16) | EXT_START_ADDR_ENABLE;
	par->ext_start_addr_hi = (base & 0x3FC00000) >> 22;

	par->pixelpipe_cfg0 = DAC_8_BIT;

	par->pixelpipe_cfg2 = DISPLAY_GAMMA_ENABLE | OVERLAY_GAMMA_ENABLE;
	par->io_cntl = EXTENDED_CRTC_CNTL;
	par->address_mapping = LINEAR_MODE_ENABLE | PAGE_MAPPING_ENABLE;
	par->display_cntl = HIRES_MODE;

	/* Set the MCLK freq */
	par->pll_cntl = PLL_MEMCLK_100000KHZ; /* 100 MHz -- use as default */

	/* Calculate the extended CRTC regs */
	par->ext_vert_total = (ytotal - 2) >> 8;
	par->ext_vert_disp_end = (yres - 1) >> 8;
	par->ext_vert_sync_start = (yres + lower) >> 8;
	par->ext_vert_blank_start = (yres + lower) >> 8;
	par->ext_horiz_total = ((xtotal >> 3) - 5) >> 8;
	par->ext_horiz_blank = (((xres + right) >> 3) & 0x40) >> 6;

	par->interlace_cntl = INTERLACE_DISABLE;

	/* Set the overscan color to 0. (NOTE: This only affects >8bpp mode) */
	par->atc[VGA_ATC_OVERSCAN] = 0;

	/* Calculate VCLK that most closely matches the requested dot clock */
	i740_calc_vclk((((u32)1e9) / var->pixclock) * (u32)(1e3), par);

	/* Since we program the clocks ourselves, always use VCLK2. */
	par->misc |= 0x0C;

	/* Calculate the FIFO Watermark and Burst Length. */
	par->lmi_fifo_watermark =
		i740_calc_fifo(par, 1000000 / var->pixclock, bpp);

	return 0;
}