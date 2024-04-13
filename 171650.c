int __sk_mem_raise_allocated(struct sock *sk, int size, int amt, int kind)
{
	struct proto *prot = sk->sk_prot;
	long allocated = sk_memory_allocated_add(sk, amt);

	if (mem_cgroup_sockets_enabled && sk->sk_memcg &&
	    !mem_cgroup_charge_skmem(sk->sk_memcg, amt))
		goto suppress_allocation;

	/* Under limit. */
	if (allocated <= sk_prot_mem_limits(sk, 0)) {
		sk_leave_memory_pressure(sk);
		return 1;
	}

	/* Under pressure. */
	if (allocated > sk_prot_mem_limits(sk, 1))
		sk_enter_memory_pressure(sk);

	/* Over hard limit. */
	if (allocated > sk_prot_mem_limits(sk, 2))
		goto suppress_allocation;

	/* guarantee minimum buffer size under pressure */
	if (kind == SK_MEM_RECV) {
		if (atomic_read(&sk->sk_rmem_alloc) < prot->sysctl_rmem[0])
			return 1;

	} else { /* SK_MEM_SEND */
		if (sk->sk_type == SOCK_STREAM) {
			if (sk->sk_wmem_queued < prot->sysctl_wmem[0])
				return 1;
		} else if (refcount_read(&sk->sk_wmem_alloc) <
			   prot->sysctl_wmem[0])
				return 1;
	}

	if (sk_has_memory_pressure(sk)) {
		int alloc;

		if (!sk_under_memory_pressure(sk))
			return 1;
		alloc = sk_sockets_allocated_read_positive(sk);
		if (sk_prot_mem_limits(sk, 2) > alloc *
		    sk_mem_pages(sk->sk_wmem_queued +
				 atomic_read(&sk->sk_rmem_alloc) +
				 sk->sk_forward_alloc))
			return 1;
	}

suppress_allocation:

	if (kind == SK_MEM_SEND && sk->sk_type == SOCK_STREAM) {
		sk_stream_moderate_sndbuf(sk);

		/* Fail only if socket is _under_ its sndbuf.
		 * In this case we cannot block, so that we have to fail.
		 */
		if (sk->sk_wmem_queued + size >= sk->sk_sndbuf)
			return 1;
	}

	trace_sock_exceed_buf_limit(sk, prot, allocated);

	sk_memory_allocated_sub(sk, amt);

	if (mem_cgroup_sockets_enabled && sk->sk_memcg)
		mem_cgroup_uncharge_skmem(sk->sk_memcg, amt);

	return 0;
}