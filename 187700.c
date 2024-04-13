static int fpga_write(int iobase, unsigned char wrd)
{
	unsigned char bit;
	int k;
	unsigned long timeout = jiffies + HZ / 10;

	for (k = 0; k < 8; k++) {
		bit = (wrd & 0x80) ? (MCR_RTS | MCR_DTR) : MCR_DTR;
		outb(bit | MCR_OUT1 | MCR_OUT2, MCR(iobase));
		wrd <<= 1;
		outb(0xfc, THR(iobase));
		while ((inb(LSR(iobase)) & LSR_TSRE) == 0)
			if (time_after(jiffies, timeout))
				return -1;
	}

	return 0;
}