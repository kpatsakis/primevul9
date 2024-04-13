static int nfc_genl_dump_ses(struct sk_buff *skb,
				 struct netlink_callback *cb)
{
	struct class_dev_iter *iter = (struct class_dev_iter *) cb->args[0];
	struct nfc_dev *dev = (struct nfc_dev *) cb->args[1];
	bool first_call = false;

	if (!iter) {
		first_call = true;
		iter = kmalloc(sizeof(struct class_dev_iter), GFP_KERNEL);
		if (!iter)
			return -ENOMEM;
		cb->args[0] = (long) iter;
	}

	mutex_lock(&nfc_devlist_mutex);

	cb->seq = nfc_devlist_generation;

	if (first_call) {
		nfc_device_iter_init(iter);
		dev = nfc_device_iter_next(iter);
	}

	while (dev) {
		int rc;

		rc = nfc_genl_send_se(skb, dev, NETLINK_CB(cb->skb).portid,
					  cb->nlh->nlmsg_seq, cb, NLM_F_MULTI);
		if (rc < 0)
			break;

		dev = nfc_device_iter_next(iter);
	}

	mutex_unlock(&nfc_devlist_mutex);

	cb->args[1] = (long) dev;

	return skb->len;
}