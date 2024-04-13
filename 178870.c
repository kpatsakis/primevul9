static int dccp_hdlr_min_cscov(struct sock *sk, u64 cscov, bool rx)
{
	struct dccp_sock *dp = dccp_sk(sk);

	if (rx)
		dp->dccps_pcrlen = cscov;
	else {
		if (dp->dccps_pcslen == 0)
			dp->dccps_pcslen = cscov;
		else if (cscov > dp->dccps_pcslen)
			DCCP_WARN("CsCov %u too small, peer requires >= %u\n",
				  dp->dccps_pcslen, (u8)cscov);
	}
	return 0;
}