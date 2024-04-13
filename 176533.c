static inline u8 i740inb(struct i740fb_par *par, u16 port)
{
	return vga_mm_r(par->regs, port);
}