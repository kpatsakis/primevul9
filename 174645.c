static int ttusb_dec_start_sec_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	struct ttusb_dec *dec = dvbdmxfeed->demux->priv;
	u8 b0[] = { 0x00, 0x00, 0x00, 0x01,
		    0x00, 0x00, 0x00, 0x00,
		    0x00, 0x00, 0x00, 0x00,
		    0x00, 0x00, 0x00, 0x00,
		    0x00, 0xff, 0x00, 0x00,
		    0x00, 0x00, 0x00, 0x00,
		    0x00, 0x00, 0x00, 0x00,
		    0x00 };
	__be16 pid;
	u8 c[COMMAND_PACKET_SIZE];
	int c_length;
	int result;
	struct filter_info *finfo;
	unsigned long flags;
	u8 x = 1;

	dprintk("%s\n", __func__);

	pid = htons(dvbdmxfeed->pid);
	memcpy(&b0[0], &pid, 2);
	memcpy(&b0[4], &x, 1);
	memcpy(&b0[5], &dvbdmxfeed->filter->filter.filter_value[0], 1);

	result = ttusb_dec_send_command(dec, 0x60, sizeof(b0), b0,
					&c_length, c);

	if (!result) {
		if (c_length == 2) {
			if (!(finfo = kmalloc(sizeof(struct filter_info),
					      GFP_ATOMIC)))
				return -ENOMEM;

			finfo->stream_id = c[1];
			finfo->filter = dvbdmxfeed->filter;

			spin_lock_irqsave(&dec->filter_info_list_lock, flags);
			list_add_tail(&finfo->filter_info_list,
				      &dec->filter_info_list);
			spin_unlock_irqrestore(&dec->filter_info_list_lock,
					       flags);

			dvbdmxfeed->priv = finfo;

			dec->filter_stream_count++;
			return ttusb_dec_start_iso_xfer(dec);
		}

		return -EAGAIN;
	} else
		return result;
}