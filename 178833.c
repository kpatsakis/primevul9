int dccp_feat_register_sp(struct sock *sk, u8 feat, u8 is_local,
			  u8 const *list, u8 len)
{	 /* any changes must be registered before establishing the connection */
	if (sk->sk_state != DCCP_CLOSED)
		return -EISCONN;
	if (dccp_feat_type(feat) != FEAT_SP)
		return -EINVAL;
	return __feat_register_sp(&dccp_sk(sk)->dccps_featneg, feat, is_local,
				  0, list, len);
}