static void get_futex_key_refs(union futex_key *key)
{
	if (!key->both.ptr)
		return;

	switch (key->both.offset & (FUT_OFF_INODE|FUT_OFF_MMSHARED)) {
	case FUT_OFF_INODE:
		ihold(key->shared.inode); /* implies smp_mb(); (B) */
		break;
	case FUT_OFF_MMSHARED:
		futex_get_mm(key); /* implies smp_mb(); (B) */
		break;
	default:
		/*
		 * Private futexes do not hold reference on an inode or
		 * mm, therefore the only purpose of calling get_futex_key_refs
		 * is because we need the barrier for the lockless waiter check.
		 */
		smp_mb(); /* explicit smp_mb(); (B) */
	}
}