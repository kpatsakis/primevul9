static int ttusb_dec_audio_pes2ts_cb(void *priv, unsigned char *data)
{
	struct ttusb_dec *dec = priv;

	dec->audio_filter->feed->cb.ts(data, 188, NULL, 0,
				       &dec->audio_filter->feed->feed.ts, NULL);

	return 0;
}