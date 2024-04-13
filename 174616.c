static int ttusb_dec_start_ts_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	struct dvb_demux *dvbdmx = dvbdmxfeed->demux;
	struct ttusb_dec *dec = dvbdmx->priv;
	u8 b0[] = { 0x05 };
	int result = 0;

	dprintk("%s\n", __func__);

	dprintk("  ts_type:");

	if (dvbdmxfeed->ts_type & TS_DECODER)
		dprintk(" TS_DECODER");

	if (dvbdmxfeed->ts_type & TS_PACKET)
		dprintk(" TS_PACKET");

	if (dvbdmxfeed->ts_type & TS_PAYLOAD_ONLY)
		dprintk(" TS_PAYLOAD_ONLY");

	dprintk("\n");

	switch (dvbdmxfeed->pes_type) {

	case DMX_PES_VIDEO:
		dprintk("  pes_type: DMX_PES_VIDEO\n");
		dec->pid[DMX_PES_PCR] = dvbdmxfeed->pid;
		dec->pid[DMX_PES_VIDEO] = dvbdmxfeed->pid;
		dec->video_filter = dvbdmxfeed->filter;
		ttusb_dec_set_pids(dec);
		break;

	case DMX_PES_AUDIO:
		dprintk("  pes_type: DMX_PES_AUDIO\n");
		dec->pid[DMX_PES_AUDIO] = dvbdmxfeed->pid;
		dec->audio_filter = dvbdmxfeed->filter;
		ttusb_dec_set_pids(dec);
		break;

	case DMX_PES_TELETEXT:
		dec->pid[DMX_PES_TELETEXT] = dvbdmxfeed->pid;
		dprintk("  pes_type: DMX_PES_TELETEXT(not supported)\n");
		return -ENOSYS;

	case DMX_PES_PCR:
		dprintk("  pes_type: DMX_PES_PCR\n");
		dec->pid[DMX_PES_PCR] = dvbdmxfeed->pid;
		ttusb_dec_set_pids(dec);
		break;

	case DMX_PES_OTHER:
		dprintk("  pes_type: DMX_PES_OTHER(not supported)\n");
		return -ENOSYS;

	default:
		dprintk("  pes_type: unknown (%d)\n", dvbdmxfeed->pes_type);
		return -EINVAL;

	}

	result = ttusb_dec_send_command(dec, 0x80, sizeof(b0), b0, NULL, NULL);
	if (result)
		return result;

	dec->pva_stream_count++;
	return ttusb_dec_start_iso_xfer(dec);
}