struct dst_entry *__sk_dst_check(struct sock *sk, u32 cookie)
{
	struct dst_entry *dst = __sk_dst_get(sk);

	if (dst && dst->obsolete && dst->ops->check(dst, cookie) == NULL) {
		sk_tx_queue_clear(sk);
		sk->sk_dst_pending_confirm = 0;
		RCU_INIT_POINTER(sk->sk_dst_cache, NULL);
		dst_release(dst);
		return NULL;
	}

	return dst;
}