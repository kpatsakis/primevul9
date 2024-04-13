static inline void *nfc_hdr_put(struct sk_buff *skb, u32 portid, u32 seq,
				int flags, u8 cmd)
{
	/* since there is no private header just add the generic one */
	return genlmsg_put(skb, portid, seq, &nfc_genl_family, flags, cmd);
}