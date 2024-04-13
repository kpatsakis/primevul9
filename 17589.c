static int snd_ctl_check_elem_info(struct snd_card *card,
				   const struct snd_ctl_elem_info *info)
{
	static const unsigned int max_value_counts[] = {
		[SNDRV_CTL_ELEM_TYPE_BOOLEAN]	= 128,
		[SNDRV_CTL_ELEM_TYPE_INTEGER]	= 128,
		[SNDRV_CTL_ELEM_TYPE_ENUMERATED] = 128,
		[SNDRV_CTL_ELEM_TYPE_BYTES]	= 512,
		[SNDRV_CTL_ELEM_TYPE_IEC958]	= 1,
		[SNDRV_CTL_ELEM_TYPE_INTEGER64] = 64,
	};

	if (info->type < SNDRV_CTL_ELEM_TYPE_BOOLEAN ||
	    info->type > SNDRV_CTL_ELEM_TYPE_INTEGER64) {
		if (card)
			dev_err(card->dev,
				"control %i:%i:%i:%s:%i: invalid type %d\n",
				info->id.iface, info->id.device,
				info->id.subdevice, info->id.name,
				info->id.index, info->type);
		return -EINVAL;
	}
	if (info->type == SNDRV_CTL_ELEM_TYPE_ENUMERATED &&
	    info->value.enumerated.items == 0) {
		if (card)
			dev_err(card->dev,
				"control %i:%i:%i:%s:%i: zero enum items\n",
				info->id.iface, info->id.device,
				info->id.subdevice, info->id.name,
				info->id.index);
		return -EINVAL;
	}
	if (info->count > max_value_counts[info->type]) {
		if (card)
			dev_err(card->dev,
				"control %i:%i:%i:%s:%i: invalid count %d\n",
				info->id.iface, info->id.device,
				info->id.subdevice, info->id.name,
				info->id.index, info->count);
		return -EINVAL;
	}

	return 0;
}