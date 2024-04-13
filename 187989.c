static inline unsigned int run_filter(const struct sk_buff *skb,
				      const struct sock *sk,
				      unsigned int res)
{
	struct sk_filter *filter;

	rcu_read_lock();
	filter = rcu_dereference(sk->sk_filter);
	if (filter != NULL)
		res = SK_RUN_FILTER(filter, skb);
	rcu_read_unlock();

	return res;
}