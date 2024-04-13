void aarp_probe_network(struct atalk_iface *atif)
{
	if (atif->dev->type == ARPHRD_LOCALTLK ||
	    atif->dev->type == ARPHRD_PPP)
		aarp_send_probe_phase1(atif);
	else {
		unsigned int count;

		for (count = 0; count < AARP_RETRANSMIT_LIMIT; count++) {
			aarp_send_probe(atif->dev, &atif->address);

			/* Defer 1/10th */
			msleep(100);

			if (atif->status & ATIF_PROBE_FAIL)
				break;
		}
	}
}