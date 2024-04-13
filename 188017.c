static void prb_run_all_ft_ops(struct kbdq_core *pkc,
			struct tpacket3_hdr *ppd)
{
	prb_fill_vlan_info(pkc, ppd);

	if (pkc->feature_req_word & TP_FT_REQ_FILL_RXHASH)
		prb_fill_rxhash(pkc, ppd);
	else
		prb_clear_rxhash(pkc, ppd);
}