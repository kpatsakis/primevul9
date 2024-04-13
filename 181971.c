static int wcd9335_trigger(struct snd_pcm_substream *substream, int cmd,
			   struct snd_soc_dai *dai)
{
	struct wcd_slim_codec_dai_data *dai_data;
	struct wcd9335_codec *wcd;
	struct slim_stream_config *cfg;

	wcd = snd_soc_component_get_drvdata(dai->component);

	dai_data = &wcd->dai[dai->id];

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		cfg = &dai_data->sconfig;
		slim_stream_prepare(dai_data->sruntime, cfg);
		slim_stream_enable(dai_data->sruntime);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		slim_stream_unprepare(dai_data->sruntime);
		slim_stream_disable(dai_data->sruntime);
		break;
	default:
		break;
	}

	return 0;
}