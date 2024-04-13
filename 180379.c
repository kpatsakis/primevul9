static int dccp_setsockopt_ccid(struct sock *sk, int type,
				char __user *optval, unsigned int optlen)
{
	u8 *val;
	int rc = 0;

	if (optlen < 1 || optlen > DCCP_FEAT_MAX_SP_VALS)
		return -EINVAL;

	val = memdup_user(optval, optlen);
	if (IS_ERR(val))
		return PTR_ERR(val);

	lock_sock(sk);
	if (type == DCCP_SOCKOPT_TX_CCID || type == DCCP_SOCKOPT_CCID)
		rc = dccp_feat_register_sp(sk, DCCPF_CCID, 1, val, optlen);

	if (!rc && (type == DCCP_SOCKOPT_RX_CCID || type == DCCP_SOCKOPT_CCID))
		rc = dccp_feat_register_sp(sk, DCCPF_CCID, 0, val, optlen);
	release_sock(sk);

	kfree(val);
	return rc;
}