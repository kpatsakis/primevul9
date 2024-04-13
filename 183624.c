void __ip_mc_inc_group(struct in_device *in_dev, __be32 addr, gfp_t gfp)
{
	____ip_mc_inc_group(in_dev, addr, MCAST_EXCLUDE, gfp);
}