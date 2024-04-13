static struct sock *sk_prot_alloc(struct proto *prot, gfp_t priority,
		int family)
{
	struct sock *sk;
	struct kmem_cache *slab;

	slab = prot->slab;
	if (slab != NULL) {
		sk = kmem_cache_alloc(slab, priority & ~__GFP_ZERO);
		if (!sk)
			return sk;
		if (priority & __GFP_ZERO)
			sk_prot_clear_nulls(sk, prot->obj_size);
	} else
		sk = kmalloc(prot->obj_size, priority);

	if (sk != NULL) {
		kmemcheck_annotate_bitfield(sk, flags);

		if (security_sk_alloc(sk, family, priority))
			goto out_free;

		if (!try_module_get(prot->owner))
			goto out_free_sec;
		sk_tx_queue_clear(sk);
	}

	return sk;

out_free_sec:
	security_sk_free(sk);
out_free:
	if (slab != NULL)
		kmem_cache_free(slab, sk);
	else
		kfree(sk);
	return NULL;
}