static inline int dccp_feat_must_be_understood(u8 feat_num)
{
	return	feat_num == DCCPF_CCID || feat_num == DCCPF_SHORT_SEQNOS ||
		feat_num == DCCPF_SEQUENCE_WINDOW;
}