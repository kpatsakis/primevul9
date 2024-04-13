isdn_readbchan_tty(int di, int channel, struct tty_port *port, int cisco_hack)
{
	int count;
	int count_pull;
	int count_put;
	int dflag;
	struct sk_buff *skb;
	char last = 0;
	int len;

	if (!dev->drv[di])
		return 0;
	if (skb_queue_empty(&dev->drv[di]->rpqueue[channel]))
		return 0;

	len = tty_buffer_request_room(port, dev->drv[di]->rcvcount[channel]);
	if (len == 0)
		return len;

	count = 0;
	while (len) {
		if (!(skb = skb_peek(&dev->drv[di]->rpqueue[channel])))
			break;
#ifdef CONFIG_ISDN_AUDIO
		if (ISDN_AUDIO_SKB_LOCK(skb))
			break;
		ISDN_AUDIO_SKB_LOCK(skb) = 1;
		if ((ISDN_AUDIO_SKB_DLECOUNT(skb)) || (dev->drv[di]->DLEflag & (1 << channel))) {
			char *p = skb->data;
			unsigned long DLEmask = (1 << channel);

			dflag = 0;
			count_pull = count_put = 0;
			while ((count_pull < skb->len) && (len > 0)) {
				/* push every character but the last to the tty buffer directly */
				if (count_put)
					tty_insert_flip_char(port, last, TTY_NORMAL);
				len--;
				if (dev->drv[di]->DLEflag & DLEmask) {
					last = DLE;
					dev->drv[di]->DLEflag &= ~DLEmask;
				} else {
					last = *p;
					if (last == DLE) {
						dev->drv[di]->DLEflag |= DLEmask;
						(ISDN_AUDIO_SKB_DLECOUNT(skb))--;
					}
					p++;
					count_pull++;
				}
				count_put++;
			}
			if (count_pull >= skb->len)
				dflag = 1;
		} else {
#endif
			/* No DLE's in buff, so simply copy it */
			dflag = 1;
			if ((count_pull = skb->len) > len) {
				count_pull = len;
				dflag = 0;
			}
			count_put = count_pull;
			if (count_put > 1)
				tty_insert_flip_string(port, skb->data, count_put - 1);
			last = skb->data[count_put - 1];
			len -= count_put;
#ifdef CONFIG_ISDN_AUDIO
		}
#endif
		count += count_put;
		if (dflag) {
			/* We got all the data in this buff.
			 * Now we can dequeue it.
			 */
			if (cisco_hack)
				tty_insert_flip_char(port, last, 0xFF);
			else
				tty_insert_flip_char(port, last, TTY_NORMAL);
#ifdef CONFIG_ISDN_AUDIO
			ISDN_AUDIO_SKB_LOCK(skb) = 0;
#endif
			skb = skb_dequeue(&dev->drv[di]->rpqueue[channel]);
			dev_kfree_skb(skb);
		} else {
			tty_insert_flip_char(port, last, TTY_NORMAL);
			/* Not yet emptied this buff, so it
			 * must stay in the queue, for further calls
			 * but we pull off the data we got until now.
			 */
			skb_pull(skb, count_pull);
#ifdef CONFIG_ISDN_AUDIO
			ISDN_AUDIO_SKB_LOCK(skb) = 0;
#endif
		}
		dev->drv[di]->rcvcount[channel] -= count_put;
	}
	return count;
}