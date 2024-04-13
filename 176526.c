static void vga_protect(struct i740fb_par *par)
{
	/* disable the display */
	i740outreg_mask(par, VGA_SEQ_I, VGA_SEQ_CLOCK_MODE, 0x20, 0x20);

	i740inb(par, 0x3DA);
	i740outb(par, VGA_ATT_W, 0x00);	/* enable palette access */
}