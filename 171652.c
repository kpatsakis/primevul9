int sock_prot_inuse_get(struct net *net, struct proto *prot)
{
	int cpu, idx = prot->inuse_idx;
	int res = 0;

	for_each_possible_cpu(cpu)
		res += per_cpu_ptr(net->core.inuse, cpu)->val[idx];

	return res >= 0 ? res : 0;
}