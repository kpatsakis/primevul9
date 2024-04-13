static ssize_t temac_show_llink_regs(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct net_device *ndev = dev_get_drvdata(dev);
	struct temac_local *lp = netdev_priv(ndev);
	int i, len = 0;

	for (i = 0; i < 0x11; i++)
		len += sprintf(buf + len, "%.8x%s", lp->dma_in(lp, i),
			       (i % 8) == 7 ? "\n" : " ");
	len += sprintf(buf + len, "\n");

	return len;
}