static void sock_copy(struct sock *nsk, const struct sock *osk)
{
#ifdef CONFIG_SECURITY_NETWORK
	void *sptr = nsk->sk_security;
#endif
	memcpy(nsk, osk, offsetof(struct sock, sk_dontcopy_begin));

	memcpy(&nsk->sk_dontcopy_end, &osk->sk_dontcopy_end,
	       osk->sk_prot->obj_size - offsetof(struct sock, sk_dontcopy_end));

#ifdef CONFIG_SECURITY_NETWORK
	nsk->sk_security = sptr;
	security_sk_clone(osk, nsk);
#endif
}