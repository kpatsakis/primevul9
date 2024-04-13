static inline u8 i740inreg(struct i740fb_par *par, u16 port, u8 reg)
{
	vga_mm_w(par->regs, port, reg);
	return vga_mm_r(par->regs, port+1);
}