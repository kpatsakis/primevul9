static void __aarp_expire_timer(struct aarp_entry **n)
{
	struct aarp_entry *t;

	while (*n)
		/* Expired ? */
		if (time_after(jiffies, (*n)->expires_at)) {
			t = *n;
			*n = (*n)->next;
			__aarp_expire(t);
		} else
			n = &((*n)->next);
}