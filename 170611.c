static void __aarp_kick(struct aarp_entry **n)
{
	struct aarp_entry *t;

	while (*n)
		/* Expired: if this will be the 11th tx, we delete instead. */
		if ((*n)->xmit_count >= sysctl_aarp_retransmit_limit) {
			t = *n;
			*n = (*n)->next;
			__aarp_expire(t);
		} else {
			__aarp_send_query(*n);
			n = &((*n)->next);
		}
}