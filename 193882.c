void tcp_syn_ack_timeout(const struct request_sock *req)
{
	struct net *net = read_pnet(&inet_rsk(req)->ireq_net);

	__NET_INC_STATS(net, LINUX_MIB_TCPTIMEOUTS);
}