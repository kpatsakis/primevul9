static void __init ip4_frags_ctl_register(void)
{
	register_net_sysctl(&init_net, "net/ipv4", ip4_frags_ctl_table);
}