static inline void i740outreg(struct i740fb_par *par, u16 port, u8 reg, u8 val)
{
	vga_mm_w_fast(par->regs, port, reg, val);
}