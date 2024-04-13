static void yam_tx_byte(struct net_device *dev, struct yam_port *yp)
{
	struct sk_buff *skb;
	unsigned char b, temp;

	switch (yp->tx_state) {
	case TX_OFF:
		break;
	case TX_HEAD:
		if (--yp->tx_count <= 0) {
			if (!(skb = skb_dequeue(&yp->send_queue))) {
				ptt_off(dev);
				yp->tx_state = TX_OFF;
				break;
			}
			yp->tx_state = TX_DATA;
			if (skb->data[0] != 0) {
/*                              do_kiss_params(s, skb->data, skb->len); */
				dev_kfree_skb_any(skb);
				break;
			}
			yp->tx_len = skb->len - 1;	/* strip KISS byte */
			if (yp->tx_len >= YAM_MAX_FRAME || yp->tx_len < 2) {
				dev_kfree_skb_any(skb);
				break;
			}
			skb_copy_from_linear_data_offset(skb, 1,
							 yp->tx_buf,
							 yp->tx_len);
			dev_kfree_skb_any(skb);
			yp->tx_count = 0;
			yp->tx_crcl = 0x21;
			yp->tx_crch = 0xf3;
			yp->tx_state = TX_DATA;
		}
		break;
	case TX_DATA:
		b = yp->tx_buf[yp->tx_count++];
		outb(b, THR(dev->base_addr));
		temp = yp->tx_crcl;
		yp->tx_crcl = chktabl[temp] ^ yp->tx_crch;
		yp->tx_crch = chktabh[temp] ^ b;
		if (yp->tx_count >= yp->tx_len) {
			yp->tx_state = TX_CRC1;
		}
		break;
	case TX_CRC1:
		yp->tx_crch = chktabl[yp->tx_crcl] ^ yp->tx_crch;
		yp->tx_crcl = chktabh[yp->tx_crcl] ^ chktabl[yp->tx_crch] ^ 0xff;
		outb(yp->tx_crcl, THR(dev->base_addr));
		yp->tx_state = TX_CRC2;
		break;
	case TX_CRC2:
		outb(chktabh[yp->tx_crch] ^ 0xFF, THR(dev->base_addr));
		if (skb_queue_empty(&yp->send_queue)) {
			yp->tx_count = (yp->bitrate * yp->txtail) / 8000;
			if (yp->dupmode == 2)
				yp->tx_count += (yp->bitrate * yp->holdd) / 8;
			if (yp->tx_count == 0)
				yp->tx_count = 1;
			yp->tx_state = TX_TAIL;
		} else {
			yp->tx_count = 1;
			yp->tx_state = TX_HEAD;
		}
		++dev->stats.tx_packets;
		break;
	case TX_TAIL:
		if (--yp->tx_count <= 0) {
			yp->tx_state = TX_OFF;
			ptt_off(dev);
		}
		break;
	}
}