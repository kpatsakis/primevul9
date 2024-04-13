static void mdesc_kfree(struct mdesc_handle *hp)
{
	BUG_ON(refcount_read(&hp->refcnt) != 0);
	BUG_ON(!list_empty(&hp->list));

	kfree(hp->self_base);
}