static int crypto_dump_report(struct sk_buff *skb, struct netlink_callback *cb)
{
	const size_t start_pos = cb->args[0];
	size_t pos = 0;
	struct crypto_dump_info info;
	struct crypto_alg *alg;
	int res;

	info.in_skb = cb->skb;
	info.out_skb = skb;
	info.nlmsg_seq = cb->nlh->nlmsg_seq;
	info.nlmsg_flags = NLM_F_MULTI;

	down_read(&crypto_alg_sem);
	list_for_each_entry(alg, &crypto_alg_list, cra_list) {
		if (pos >= start_pos) {
			res = crypto_report_alg(alg, &info);
			if (res == -EMSGSIZE)
				break;
			if (res)
				goto out;
		}
		pos++;
	}
	cb->args[0] = pos;
	res = skb->len;
out:
	up_read(&crypto_alg_sem);
	return res;
}