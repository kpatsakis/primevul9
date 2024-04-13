static int i740fb_set_par(struct fb_info *info)
{
	struct i740fb_par *par = info->par;
	u32 itemp;
	int i;

	i = i740fb_decode_var(&info->var, par, info);
	if (i)
		return i;

	memset_io(info->screen_base, 0, info->screen_size);

	vga_protect(par);

	i740outreg(par, XRX, DRAM_EXT_CNTL, DRAM_REFRESH_DISABLE);

	mdelay(1);

	i740outreg(par, XRX, VCLK2_VCO_M, par->video_clk2_m);
	i740outreg(par, XRX, VCLK2_VCO_N, par->video_clk2_n);
	i740outreg(par, XRX, VCLK2_VCO_MN_MSBS, par->video_clk2_mn_msbs);
	i740outreg(par, XRX, VCLK2_VCO_DIV_SEL, par->video_clk2_div_sel);

	i740outreg_mask(par, XRX, PIXPIPE_CONFIG_0,
			par->pixelpipe_cfg0 & DAC_8_BIT, 0x80);

	i740inb(par, 0x3DA);
	i740outb(par, 0x3C0, 0x00);

	/* update misc output register */
	i740outb(par, VGA_MIS_W, par->misc | 0x01);

	/* synchronous reset on */
	i740outreg(par, VGA_SEQ_I, VGA_SEQ_RESET, 0x01);
	/* write sequencer registers */
	i740outreg(par, VGA_SEQ_I, VGA_SEQ_CLOCK_MODE,
			par->seq[VGA_SEQ_CLOCK_MODE] | 0x20);
	for (i = 2; i < VGA_SEQ_C; i++)
		i740outreg(par, VGA_SEQ_I, i, par->seq[i]);

	/* synchronous reset off */
	i740outreg(par, VGA_SEQ_I, VGA_SEQ_RESET, 0x03);

	/* deprotect CRT registers 0-7 */
	i740outreg(par, VGA_CRT_IC, VGA_CRTC_V_SYNC_END,
			par->crtc[VGA_CRTC_V_SYNC_END]);

	/* write CRT registers */
	for (i = 0; i < VGA_CRT_C; i++)
		i740outreg(par, VGA_CRT_IC, i, par->crtc[i]);

	/* write graphics controller registers */
	for (i = 0; i < VGA_GFX_C; i++)
		i740outreg(par, VGA_GFX_I, i, par->gdc[i]);

	/* write attribute controller registers */
	for (i = 0; i < VGA_ATT_C; i++) {
		i740inb(par, VGA_IS1_RC);		/* reset flip-flop */
		i740outb(par, VGA_ATT_IW, i);
		i740outb(par, VGA_ATT_IW, par->atc[i]);
	}

	i740inb(par, VGA_IS1_RC);
	i740outb(par, VGA_ATT_IW, 0x20);

	i740outreg(par, VGA_CRT_IC, EXT_VERT_TOTAL, par->ext_vert_total);
	i740outreg(par, VGA_CRT_IC, EXT_VERT_DISPLAY, par->ext_vert_disp_end);
	i740outreg(par, VGA_CRT_IC, EXT_VERT_SYNC_START,
			par->ext_vert_sync_start);
	i740outreg(par, VGA_CRT_IC, EXT_VERT_BLANK_START,
			par->ext_vert_blank_start);
	i740outreg(par, VGA_CRT_IC, EXT_HORIZ_TOTAL, par->ext_horiz_total);
	i740outreg(par, VGA_CRT_IC, EXT_HORIZ_BLANK, par->ext_horiz_blank);
	i740outreg(par, VGA_CRT_IC, EXT_OFFSET, par->ext_offset);
	i740outreg(par, VGA_CRT_IC, EXT_START_ADDR_HI, par->ext_start_addr_hi);
	i740outreg(par, VGA_CRT_IC, EXT_START_ADDR, par->ext_start_addr);

	i740outreg_mask(par, VGA_CRT_IC, INTERLACE_CNTL,
			par->interlace_cntl, INTERLACE_ENABLE);
	i740outreg_mask(par, XRX, ADDRESS_MAPPING, par->address_mapping, 0x1F);
	i740outreg_mask(par, XRX, BITBLT_CNTL, par->bitblt_cntl, COLEXP_MODE);
	i740outreg_mask(par, XRX, DISPLAY_CNTL,
			par->display_cntl, VGA_WRAP_MODE | GUI_MODE);
	i740outreg_mask(par, XRX, PIXPIPE_CONFIG_0, par->pixelpipe_cfg0, 0x9B);
	i740outreg_mask(par, XRX, PIXPIPE_CONFIG_2, par->pixelpipe_cfg2, 0x0C);

	i740outreg(par, XRX, PLL_CNTL, par->pll_cntl);

	i740outreg_mask(par, XRX, PIXPIPE_CONFIG_1,
			par->pixelpipe_cfg1, DISPLAY_COLOR_MODE);

	itemp = readl(par->regs + FWATER_BLC);
	itemp &= ~(LMI_BURST_LENGTH | LMI_FIFO_WATERMARK);
	itemp |= par->lmi_fifo_watermark;
	writel(itemp, par->regs + FWATER_BLC);

	i740outreg(par, XRX, DRAM_EXT_CNTL, DRAM_REFRESH_60HZ);

	i740outreg_mask(par, MRX, COL_KEY_CNTL_1, 0, BLANK_DISP_OVERLAY);
	i740outreg_mask(par, XRX, IO_CTNL,
			par->io_cntl, EXTENDED_ATTR_CNTL | EXTENDED_CRTC_CNTL);

	if (par->pixelpipe_cfg1 != DISPLAY_8BPP_MODE) {
		i740outb(par, VGA_PEL_MSK, 0xFF);
		i740outb(par, VGA_PEL_IW, 0x00);
		for (i = 0; i < 256; i++) {
			itemp = (par->pixelpipe_cfg0 & DAC_8_BIT) ? i : i >> 2;
			i740outb(par, VGA_PEL_D, itemp);
			i740outb(par, VGA_PEL_D, itemp);
			i740outb(par, VGA_PEL_D, itemp);
		}
	}

	/* Wait for screen to stabilize. */
	mdelay(50);
	vga_unprotect(par);

	info->fix.line_length =
			info->var.xres_virtual * info->var.bits_per_pixel / 8;
	if (info->var.bits_per_pixel == 8)
		info->fix.visual = FB_VISUAL_PSEUDOCOLOR;
	else
		info->fix.visual = FB_VISUAL_TRUECOLOR;

	return 0;
}