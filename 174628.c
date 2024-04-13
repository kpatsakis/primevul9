static int ttusb_dec_stop_sec_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	struct ttusb_dec *dec = dvbdmxfeed->demux->priv;
	u8 b0[] = { 0x00, 0x00 };
	struct filter_info *finfo = (struct filter_info *)dvbdmxfeed->priv;
	unsigned long flags;

	b0[1] = finfo->stream_id;
	spin_lock_irqsave(&dec->filter_info_list_lock, flags);
	list_del(&finfo->filter_info_list);
	spin_unlock_irqrestore(&dec->filter_info_list_lock, flags);
	kfree(finfo);
	ttusb_dec_send_command(dec, 0x62, sizeof(b0), b0, NULL, NULL);

	dec->filter_stream_count--;

	ttusb_dec_stop_iso_xfer(dec);

	return 0;
}