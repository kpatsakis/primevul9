static struct aarp_entry *aarp_alloc(void)
{
	struct aarp_entry *a = kmalloc(sizeof(*a), GFP_ATOMIC);

	if (a)
		skb_queue_head_init(&a->packet_queue);
	return a;
}