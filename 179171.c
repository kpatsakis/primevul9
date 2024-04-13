int nfc_genl_llc_send_sdres(struct nfc_dev *dev, struct hlist_head *sdres_list)
{
	struct sk_buff *msg;
	struct nlattr *sdp_attr, *uri_attr;
	struct nfc_llcp_sdp_tlv *sdres;
	struct hlist_node *n;
	void *hdr;
	int rc = -EMSGSIZE;
	int i;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return -ENOMEM;

	hdr = genlmsg_put(msg, 0, 0, &nfc_genl_family, 0,
			  NFC_EVENT_LLC_SDRES);
	if (!hdr)
		goto free_msg;

	if (nla_put_u32(msg, NFC_ATTR_DEVICE_INDEX, dev->idx))
		goto nla_put_failure;

	sdp_attr = nla_nest_start_noflag(msg, NFC_ATTR_LLC_SDP);
	if (sdp_attr == NULL) {
		rc = -ENOMEM;
		goto nla_put_failure;
	}

	i = 1;
	hlist_for_each_entry_safe(sdres, n, sdres_list, node) {
		pr_debug("uri: %s, sap: %d\n", sdres->uri, sdres->sap);

		uri_attr = nla_nest_start_noflag(msg, i++);
		if (uri_attr == NULL) {
			rc = -ENOMEM;
			goto nla_put_failure;
		}

		if (nla_put_u8(msg, NFC_SDP_ATTR_SAP, sdres->sap))
			goto nla_put_failure;

		if (nla_put_string(msg, NFC_SDP_ATTR_URI, sdres->uri))
			goto nla_put_failure;

		nla_nest_end(msg, uri_attr);

		hlist_del(&sdres->node);

		nfc_llcp_free_sdp_tlv(sdres);
	}

	nla_nest_end(msg, sdp_attr);

	genlmsg_end(msg, hdr);

	return genlmsg_multicast(&nfc_genl_family, msg, 0, 0, GFP_ATOMIC);

nla_put_failure:
free_msg:
	nlmsg_free(msg);

	nfc_llcp_free_sdp_tlv_list(sdres_list);

	return rc;
}