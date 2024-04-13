static inline void i740outreg_mask(struct i740fb_par *par, u16 port, u8 reg,
				   u8 val, u8 mask)
{
	vga_mm_w_fast(par->regs, port, reg, (val & mask)
		| (i740inreg(par, port, reg) & ~mask));
}