int __init atalk_proc_init(void)
{
	if (!proc_mkdir("atalk", init_net.proc_net))
		return -ENOMEM;

	if (!proc_create_seq("atalk/interface", 0444, init_net.proc_net,
			    &atalk_seq_interface_ops))
		goto out;

	if (!proc_create_seq("atalk/route", 0444, init_net.proc_net,
			    &atalk_seq_route_ops))
		goto out;

	if (!proc_create_seq("atalk/socket", 0444, init_net.proc_net,
			    &atalk_seq_socket_ops))
		goto out;

	if (!proc_create_seq_private("atalk/arp", 0444, init_net.proc_net,
				     &aarp_seq_ops,
				     sizeof(struct aarp_iter_state), NULL))
		goto out;

out:
	remove_proc_subtree("atalk", init_net.proc_net);
	return -ENOMEM;
}