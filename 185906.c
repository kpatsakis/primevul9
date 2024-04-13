void rt6_clean_tohost(struct net *net, struct in6_addr *gateway)
{
	fib6_clean_all(net, fib6_clean_tohost, gateway);
}