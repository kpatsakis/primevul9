static void ptt_on(struct net_device *dev)
{
	outb(PTT_ON, MCR(dev->base_addr));
}