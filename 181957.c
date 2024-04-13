static int wcd9335_hw_params(struct snd_pcm_substream *substream,
			   struct snd_pcm_hw_params *params,
			   struct snd_soc_dai *dai)
{
	struct wcd9335_codec *wcd;
	int ret, tx_fs_rate = 0;

	wcd = snd_soc_component_get_drvdata(dai->component);

	switch (substream->stream) {
	case SNDRV_PCM_STREAM_PLAYBACK:
		ret = wcd9335_set_interpolator_rate(dai, params_rate(params));
		if (ret) {
			dev_err(wcd->dev, "cannot set sample rate: %u\n",
				params_rate(params));
			return ret;
		}
		switch (params_width(params)) {
		case 16 ... 24:
			wcd->dai[dai->id].sconfig.bps = params_width(params);
			break;
		default:
			dev_err(wcd->dev, "%s: Invalid format 0x%x\n",
				__func__, params_width(params));
			return -EINVAL;
		}
		break;

	case SNDRV_PCM_STREAM_CAPTURE:
		switch (params_rate(params)) {
		case 8000:
			tx_fs_rate = 0;
			break;
		case 16000:
			tx_fs_rate = 1;
			break;
		case 32000:
			tx_fs_rate = 3;
			break;
		case 48000:
			tx_fs_rate = 4;
			break;
		case 96000:
			tx_fs_rate = 5;
			break;
		case 192000:
			tx_fs_rate = 6;
			break;
		case 384000:
			tx_fs_rate = 7;
			break;
		default:
			dev_err(wcd->dev, "%s: Invalid TX sample rate: %d\n",
				__func__, params_rate(params));
			return -EINVAL;

		};

		ret = wcd9335_set_decimator_rate(dai, tx_fs_rate,
						params_rate(params));
		if (ret < 0) {
			dev_err(wcd->dev, "Cannot set TX Decimator rate\n");
			return ret;
		}
		switch (params_width(params)) {
		case 16 ... 32:
			wcd->dai[dai->id].sconfig.bps = params_width(params);
			break;
		default:
			dev_err(wcd->dev, "%s: Invalid format 0x%x\n",
				__func__, params_width(params));
			return -EINVAL;
		};
		break;
	default:
		dev_err(wcd->dev, "Invalid stream type %d\n",
			substream->stream);
		return -EINVAL;
	};

	wcd->dai[dai->id].sconfig.rate = params_rate(params);
	wcd9335_slim_set_hw_params(wcd, &wcd->dai[dai->id], substream->stream);

	return 0;
}