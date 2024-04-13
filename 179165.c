int nfc_genl_se_transaction(struct nfc_dev *dev, u8 se_idx,
			    struct nfc_evt_transaction *evt_transaction)
{
	struct nfc_se *se;
	struct sk_buff *msg;
	void *hdr;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return -ENOMEM;

	hdr = genlmsg_put(msg, 0, 0, &nfc_genl_family, 0,
			  NFC_EVENT_SE_TRANSACTION);
	if (!hdr)
		goto free_msg;

	se = nfc_find_se(dev, se_idx);
	if (!se)
		goto free_msg;

	if (nla_put_u32(msg, NFC_ATTR_DEVICE_INDEX, dev->idx) ||
	    nla_put_u32(msg, NFC_ATTR_SE_INDEX, se_idx) ||
	    nla_put_u8(msg, NFC_ATTR_SE_TYPE, se->type) ||
	    nla_put(msg, NFC_ATTR_SE_AID, evt_transaction->aid_len,
		    evt_transaction->aid) ||
	    nla_put(msg, NFC_ATTR_SE_PARAMS, evt_transaction->params_len,
		    evt_transaction->params))
		goto nla_put_failure;

	/* evt_transaction is no more used */
	devm_kfree(&dev->dev, evt_transaction);

	genlmsg_end(msg, hdr);

	genlmsg_multicast(&nfc_genl_family, msg, 0, 0, GFP_KERNEL);

	return 0;

nla_put_failure:
free_msg:
	/* evt_transaction is no more used */
	devm_kfree(&dev->dev, evt_transaction);
	nlmsg_free(msg);
	return -EMSGSIZE;
}