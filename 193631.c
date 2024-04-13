static int crypto_report_comp(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_report_comp rcomp;

	memset(&rcomp, 0, sizeof(rcomp));

	strscpy(rcomp.type, "compression", sizeof(rcomp.type));

	return nla_put(skb, CRYPTOCFGA_REPORT_COMPRESS, sizeof(rcomp), &rcomp);
}