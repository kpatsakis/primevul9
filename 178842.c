static u8 dccp_feat_is_valid_sp_val(u8 feat_num, u8 val)
{
	switch (feat_num) {
	case DCCPF_CCID:
		return val == DCCPC_CCID2 || val == DCCPC_CCID3;
	/* Type-check Boolean feature values: */
	case DCCPF_SHORT_SEQNOS:
	case DCCPF_ECN_INCAPABLE:
	case DCCPF_SEND_ACK_VECTOR:
	case DCCPF_SEND_NDP_COUNT:
	case DCCPF_DATA_CHECKSUM:
	case DCCPF_SEND_LEV_RATE:
		return val < 2;
	case DCCPF_MIN_CSUM_COVER:
		return val < 16;
	}
	return 0;			/* feature unknown */
}