static int llsec_do_encrypt(struct sk_buff *skb,
			    const struct mac802154_llsec *sec,
			    const struct ieee802154_hdr *hdr,
			    struct mac802154_llsec_key *key)
{
	if (hdr->sec.level == IEEE802154_SCF_SECLEVEL_ENC)
		return llsec_do_encrypt_unauth(skb, sec, hdr, key);
	else
		return llsec_do_encrypt_auth(skb, sec, hdr, key);
}