static void dump_firm_type_and_int_freq(unsigned int type, u16 int_freq)
{
	if (type & BASE)
		printk("BASE ");
	if (type & INIT1)
		printk("INIT1 ");
	if (type & F8MHZ)
		printk("F8MHZ ");
	if (type & MTS)
		printk("MTS ");
	if (type & D2620)
		printk("D2620 ");
	if (type & D2633)
		printk("D2633 ");
	if (type & DTV6)
		printk("DTV6 ");
	if (type & QAM)
		printk("QAM ");
	if (type & DTV7)
		printk("DTV7 ");
	if (type & DTV78)
		printk("DTV78 ");
	if (type & DTV8)
		printk("DTV8 ");
	if (type & FM)
		printk("FM ");
	if (type & INPUT1)
		printk("INPUT1 ");
	if (type & LCD)
		printk("LCD ");
	if (type & NOGD)
		printk("NOGD ");
	if (type & MONO)
		printk("MONO ");
	if (type & ATSC)
		printk("ATSC ");
	if (type & IF)
		printk("IF ");
	if (type & LG60)
		printk("LG60 ");
	if (type & ATI638)
		printk("ATI638 ");
	if (type & OREN538)
		printk("OREN538 ");
	if (type & OREN36)
		printk("OREN36 ");
	if (type & TOYOTA388)
		printk("TOYOTA388 ");
	if (type & TOYOTA794)
		printk("TOYOTA794 ");
	if (type & DIBCOM52)
		printk("DIBCOM52 ");
	if (type & ZARLINK456)
		printk("ZARLINK456 ");
	if (type & CHINA)
		printk("CHINA ");
	if (type & F6MHZ)
		printk("F6MHZ ");
	if (type & INPUT2)
		printk("INPUT2 ");
	if (type & SCODE)
		printk("SCODE ");
	if (type & HAS_IF)
		printk("HAS_IF_%d ", int_freq);
}