static int ttusb_dec_stop_ts_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	struct ttusb_dec *dec = dvbdmxfeed->demux->priv;
	u8 b0[] = { 0x00 };

	ttusb_dec_send_command(dec, 0x81, sizeof(b0), b0, NULL, NULL);

	dec->pva_stream_count--;

	ttusb_dec_stop_iso_xfer(dec);

	return 0;
}