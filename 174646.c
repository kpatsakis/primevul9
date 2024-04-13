static int ttusb_dec_start_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	struct dvb_demux *dvbdmx = dvbdmxfeed->demux;

	dprintk("%s\n", __func__);

	if (!dvbdmx->dmx.frontend)
		return -EINVAL;

	dprintk("  pid: 0x%04X\n", dvbdmxfeed->pid);

	switch (dvbdmxfeed->type) {

	case DMX_TYPE_TS:
		return ttusb_dec_start_ts_feed(dvbdmxfeed);
		break;

	case DMX_TYPE_SEC:
		return ttusb_dec_start_sec_feed(dvbdmxfeed);
		break;

	default:
		dprintk("  type: unknown (%d)\n", dvbdmxfeed->type);
		return -EINVAL;

	}
}