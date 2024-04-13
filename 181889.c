static void recv_handler(struct work_struct *work)
{
	struct sk_buff *skb;
	struct capi20_appl *ap =
		container_of(work, struct capi20_appl, recv_work);

	if ((!ap) || (ap->release_in_progress))
		return;

	mutex_lock(&ap->recv_mtx);
	while ((skb = skb_dequeue(&ap->recv_queue))) {
		if (CAPIMSG_CMD(skb->data) == CAPI_DATA_B3_IND)
			ap->nrecvdatapkt++;
		else
			ap->nrecvctlpkt++;

		ap->recv_message(ap, skb);
	}
	mutex_unlock(&ap->recv_mtx);
}