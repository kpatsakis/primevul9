llsec_do_decrypt(struct sk_buff *skb, const struct mac802154_llsec *sec,
		 const struct ieee802154_hdr *hdr,
		 struct mac802154_llsec_key *key, __le64 dev_addr)
{
	if (hdr->sec.level == IEEE802154_SCF_SECLEVEL_ENC)
		return llsec_do_decrypt_unauth(skb, sec, hdr, key, dev_addr);
	else
		return llsec_do_decrypt_auth(skb, sec, hdr, key, dev_addr);
}