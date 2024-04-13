static void vga_unprotect(struct i740fb_par *par)
{
	/* reenable display */
	i740outreg_mask(par, VGA_SEQ_I, VGA_SEQ_CLOCK_MODE, 0, 0x20);

	i740inb(par, 0x3DA);
	i740outb(par, VGA_ATT_W, 0x20);	/* disable palette access */
}