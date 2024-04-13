isdn_net_rmallphone(isdn_net_dev *p)
{
	isdn_net_phone *n;
	isdn_net_phone *m;
	int i;

	for (i = 0; i < 2; i++) {
		n = p->local->phone[i];
		while (n) {
			m = n->next;
			kfree(n);
			n = m;
		}
		p->local->phone[i] = NULL;
	}
	p->local->dial = NULL;
	return 0;
}