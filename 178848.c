static int dccp_feat_index(u8 feat_num)
{
	/* The first 9 entries are occupied by the types from RFC 4340, 6.4 */
	if (feat_num > DCCPF_RESERVED && feat_num <= DCCPF_DATA_CHECKSUM)
		return feat_num - 1;

	/*
	 * Other features: add cases for new feature types here after adding
	 * them to the above table.
	 */
	switch (feat_num) {
	case DCCPF_SEND_LEV_RATE:
			return DCCP_FEAT_SUPPORTED_MAX - 1;
	}
	return -1;
}