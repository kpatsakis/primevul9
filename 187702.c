static void ptt_off(struct net_device *dev)
{
	outb(PTT_OFF, MCR(dev->base_addr));
}