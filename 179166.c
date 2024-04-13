static void se_io_cb(void *context, u8 *apdu, size_t apdu_len, int err)
{
	struct se_io_ctx *ctx = context;
	struct sk_buff *msg;
	void *hdr;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg) {
		kfree(ctx);
		return;
	}

	hdr = genlmsg_put(msg, 0, 0, &nfc_genl_family, 0,
			  NFC_CMD_SE_IO);
	if (!hdr)
		goto free_msg;

	if (nla_put_u32(msg, NFC_ATTR_DEVICE_INDEX, ctx->dev_idx) ||
	    nla_put_u32(msg, NFC_ATTR_SE_INDEX, ctx->se_idx) ||
	    nla_put(msg, NFC_ATTR_SE_APDU, apdu_len, apdu))
		goto nla_put_failure;

	genlmsg_end(msg, hdr);

	genlmsg_multicast(&nfc_genl_family, msg, 0, 0, GFP_KERNEL);

	kfree(ctx);

	return;

nla_put_failure:
free_msg:
	nlmsg_free(msg);
	kfree(ctx);

	return;
}