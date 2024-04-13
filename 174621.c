static void ttusb_dec_process_filter(struct ttusb_dec *dec, u8 *packet,
				     int length)
{
	struct list_head *item;
	struct filter_info *finfo;
	struct dvb_demux_filter *filter = NULL;
	unsigned long flags;
	u8 sid;

	sid = packet[1];
	spin_lock_irqsave(&dec->filter_info_list_lock, flags);
	for (item = dec->filter_info_list.next; item != &dec->filter_info_list;
	     item = item->next) {
		finfo = list_entry(item, struct filter_info, filter_info_list);
		if (finfo->stream_id == sid) {
			filter = finfo->filter;
			break;
		}
	}
	spin_unlock_irqrestore(&dec->filter_info_list_lock, flags);

	if (filter)
		filter->feed->cb.sec(&packet[2], length - 2, NULL, 0,
				     &filter->filter, NULL);
}