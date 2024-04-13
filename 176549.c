static inline int cipso_v4_skbuff_setattr(struct sk_buff *skb,
				      const struct cipso_v4_doi *doi_def,
				      const struct netlbl_lsm_secattr *secattr)
{
	return -ENOSYS;
}