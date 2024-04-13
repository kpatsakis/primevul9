static int ttusb_dec_stop_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	dprintk("%s\n", __func__);

	switch (dvbdmxfeed->type) {
	case DMX_TYPE_TS:
		return ttusb_dec_stop_ts_feed(dvbdmxfeed);
		break;

	case DMX_TYPE_SEC:
		return ttusb_dec_stop_sec_feed(dvbdmxfeed);
		break;
	}

	return 0;
}