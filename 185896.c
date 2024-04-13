void rt6_ifdown(struct net *net, struct net_device *dev)
{
	struct arg_dev_net adn = {
		.dev = dev,
		.net = net,
	};

	fib6_clean_all(net, fib6_ifdown, &adn);
	icmp6_clean_all(fib6_ifdown, &adn);
}