static inline void __sock_kfree_s(struct sock *sk, void *mem, int size,
				  const bool nullify)
{
	if (WARN_ON_ONCE(!mem))
		return;
	if (nullify)
		kzfree(mem);
	else
		kfree(mem);
	atomic_sub(size, &sk->sk_omem_alloc);
}