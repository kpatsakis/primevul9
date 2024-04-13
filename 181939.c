static int wcd9335_codec_enable_micbias(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kc, int event)
{
	return __wcd9335_codec_enable_micbias(w, event);
}