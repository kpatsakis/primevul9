static u8 dccp_feat_valid_nn_length(u8 feat_num)
{
	if (feat_num == DCCPF_ACK_RATIO)	/* RFC 4340, 11.3 and 6.6.8 */
		return 2;
	if (feat_num == DCCPF_SEQUENCE_WINDOW)	/* RFC 4340, 7.5.2 and 6.5  */
		return 6;
	return 0;
}