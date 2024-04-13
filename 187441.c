static void mdesc_handle_init(struct mdesc_handle *hp,
			      unsigned int handle_size,
			      void *base)
{
	BUG_ON(((unsigned long)&hp->mdesc) & (16UL - 1));

	memset(hp, 0, handle_size);
	INIT_LIST_HEAD(&hp->list);
	hp->self_base = base;
	refcount_set(&hp->refcnt, 1);
	hp->handle_size = handle_size;
}