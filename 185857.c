static struct fib6_node* fib6_backtrack(struct fib6_node *fn,
					struct in6_addr *saddr)
{
	struct fib6_node *pn;
	while (1) {
		if (fn->fn_flags & RTN_TL_ROOT)
			return NULL;
		pn = fn->parent;
		if (FIB6_SUBTREE(pn) && FIB6_SUBTREE(pn) != fn)
			fn = fib6_lookup(FIB6_SUBTREE(pn), NULL, saddr);
		else
			fn = pn;
		if (fn->fn_flags & RTN_RTINFO)
			return fn;
	}
}