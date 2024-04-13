void mdesc_release(struct mdesc_handle *hp)
{
	unsigned long flags;

	spin_lock_irqsave(&mdesc_lock, flags);
	if (refcount_dec_and_test(&hp->refcnt)) {
		list_del_init(&hp->list);
		hp->mops->free(hp);
	}
	spin_unlock_irqrestore(&mdesc_lock, flags);
}