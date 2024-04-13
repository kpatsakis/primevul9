verdicthdr_get(const struct nlattr * const nfqa[])
{
	struct nfqnl_msg_verdict_hdr *vhdr;
	unsigned int verdict;

	if (!nfqa[NFQA_VERDICT_HDR])
		return NULL;

	vhdr = nla_data(nfqa[NFQA_VERDICT_HDR]);
	verdict = ntohl(vhdr->verdict) & NF_VERDICT_MASK;
	if (verdict > NF_MAX_VERDICT || verdict == NF_STOLEN)
		return NULL;
	return vhdr;
}