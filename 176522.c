static inline void i740outb(struct i740fb_par *par, u16 port, u8 val)
{
	vga_mm_w(par->regs, port, val);
}