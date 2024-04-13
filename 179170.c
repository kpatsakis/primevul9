static int nfc_genl_send_target(struct sk_buff *msg, struct nfc_target *target,
				struct netlink_callback *cb, int flags)
{
	void *hdr;

	hdr = genlmsg_put(msg, NETLINK_CB(cb->skb).portid, cb->nlh->nlmsg_seq,
			  &nfc_genl_family, flags, NFC_CMD_GET_TARGET);
	if (!hdr)
		return -EMSGSIZE;

	genl_dump_check_consistent(cb, hdr);

	if (nla_put_u32(msg, NFC_ATTR_TARGET_INDEX, target->idx) ||
	    nla_put_u32(msg, NFC_ATTR_PROTOCOLS, target->supported_protocols) ||
	    nla_put_u16(msg, NFC_ATTR_TARGET_SENS_RES, target->sens_res) ||
	    nla_put_u8(msg, NFC_ATTR_TARGET_SEL_RES, target->sel_res))
		goto nla_put_failure;
	if (target->nfcid1_len > 0 &&
	    nla_put(msg, NFC_ATTR_TARGET_NFCID1, target->nfcid1_len,
		    target->nfcid1))
		goto nla_put_failure;
	if (target->sensb_res_len > 0 &&
	    nla_put(msg, NFC_ATTR_TARGET_SENSB_RES, target->sensb_res_len,
		    target->sensb_res))
		goto nla_put_failure;
	if (target->sensf_res_len > 0 &&
	    nla_put(msg, NFC_ATTR_TARGET_SENSF_RES, target->sensf_res_len,
		    target->sensf_res))
		goto nla_put_failure;

	if (target->is_iso15693) {
		if (nla_put_u8(msg, NFC_ATTR_TARGET_ISO15693_DSFID,
			       target->iso15693_dsfid) ||
		    nla_put(msg, NFC_ATTR_TARGET_ISO15693_UID,
			    sizeof(target->iso15693_uid), target->iso15693_uid))
			goto nla_put_failure;
	}

	genlmsg_end(msg, hdr);
	return 0;

nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}