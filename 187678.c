static void fpga_reset(int iobase)
{
	outb(0, IER(iobase));
	outb(LCR_DLAB | LCR_BIT5, LCR(iobase));
	outb(1, DLL(iobase));
	outb(0, DLM(iobase));

	outb(LCR_BIT5, LCR(iobase));
	inb(LSR(iobase));
	inb(MSR(iobase));
	/* turn off FPGA supply voltage */
	outb(MCR_OUT1 | MCR_OUT2, MCR(iobase));
	delay(100);
	/* turn on FPGA supply voltage again */
	outb(MCR_DTR | MCR_RTS | MCR_OUT1 | MCR_OUT2, MCR(iobase));
	delay(100);
}