void ip_mc_inc_group(struct in_device *in_dev, __be32 addr)
{
	__ip_mc_inc_group(in_dev, addr, GFP_KERNEL);
}