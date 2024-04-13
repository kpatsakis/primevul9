static ssize_t show_pcm_class(struct device *dev,
			      struct device_attribute *attr, char *buf)
{
	struct snd_pcm_str *pstr = container_of(dev, struct snd_pcm_str, dev);
	struct snd_pcm *pcm = pstr->pcm;
	const char *str;
	static const char *strs[SNDRV_PCM_CLASS_LAST + 1] = {
		[SNDRV_PCM_CLASS_GENERIC] = "generic",
		[SNDRV_PCM_CLASS_MULTI] = "multi",
		[SNDRV_PCM_CLASS_MODEM] = "modem",
		[SNDRV_PCM_CLASS_DIGITIZER] = "digitizer",
	};

	if (pcm->dev_class > SNDRV_PCM_CLASS_LAST)
		str = "none";
	else
		str = strs[pcm->dev_class];
        return snprintf(buf, PAGE_SIZE, "%s\n", str);
}