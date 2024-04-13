void *idr_replace(struct idr *idp, void *ptr, int id)
{
	int n;
	struct idr_layer *p, *old_p;

	p = idp->top;
	if (!p)
		return ERR_PTR(-EINVAL);

	n = (p->layer+1) * IDR_BITS;

	id &= MAX_ID_MASK;

	if (id >= (1 << n))
		return ERR_PTR(-EINVAL);

	n -= IDR_BITS;
	while ((n > 0) && p) {
		p = p->ary[(id >> n) & IDR_MASK];
		n -= IDR_BITS;
	}

	n = id & IDR_MASK;
	if (unlikely(p == NULL || !test_bit(n, &p->bitmap)))
		return ERR_PTR(-ENOENT);

	old_p = p->ary[n];
	rcu_assign_pointer(p->ary[n], ptr);

	return old_p;
}