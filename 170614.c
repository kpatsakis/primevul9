static void __aarp_expire_all(struct aarp_entry **n)
{
	struct aarp_entry *t;

	while (*n) {
		t = *n;
		*n = (*n)->next;
		__aarp_expire(t);
	}
}