static void ttusb_dec_set_pids(struct ttusb_dec *dec)
{
	u8 b[] = { 0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0xff, 0xff,
		   0xff, 0xff, 0xff, 0xff };

	__be16 pcr = htons(dec->pid[DMX_PES_PCR]);
	__be16 audio = htons(dec->pid[DMX_PES_AUDIO]);
	__be16 video = htons(dec->pid[DMX_PES_VIDEO]);

	dprintk("%s\n", __func__);

	memcpy(&b[0], &pcr, 2);
	memcpy(&b[2], &audio, 2);
	memcpy(&b[4], &video, 2);

	ttusb_dec_send_command(dec, 0x50, sizeof(b), b, NULL, NULL);

	dvb_filter_pes2ts_init(&dec->a_pes2ts, dec->pid[DMX_PES_AUDIO],
			       ttusb_dec_audio_pes2ts_cb, dec);
	dvb_filter_pes2ts_init(&dec->v_pes2ts, dec->pid[DMX_PES_VIDEO],
			       ttusb_dec_video_pes2ts_cb, dec);
	dec->v_pes_length = 0;
	dec->v_pes_postbytes = 0;
}