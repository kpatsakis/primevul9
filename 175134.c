static int nfqnl_put_sk_uidgid(struct sk_buff *skb, struct sock *sk)
{
	const struct cred *cred;

	if (sk->sk_state == TCP_TIME_WAIT)
		return 0;

	read_lock_bh(&sk->sk_callback_lock);
	if (sk->sk_socket && sk->sk_socket->file) {
		cred = sk->sk_socket->file->f_cred;
		if (nla_put_be32(skb, NFQA_UID,
		    htonl(from_kuid_munged(&init_user_ns, cred->fsuid))))
			goto nla_put_failure;
		if (nla_put_be32(skb, NFQA_GID,
		    htonl(from_kgid_munged(&init_user_ns, cred->fsgid))))
			goto nla_put_failure;
	}
	read_unlock_bh(&sk->sk_callback_lock);
	return 0;

nla_put_failure:
	read_unlock_bh(&sk->sk_callback_lock);
	return -1;
}