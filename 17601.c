static int sanity_check_input_values(struct snd_card *card,
				     const struct snd_ctl_elem_value *control,
				     const struct snd_ctl_elem_info *info,
				     bool print_error)
{
	int i, ret;

	switch (info->type) {
	case SNDRV_CTL_ELEM_TYPE_BOOLEAN:
	case SNDRV_CTL_ELEM_TYPE_INTEGER:
	case SNDRV_CTL_ELEM_TYPE_INTEGER64:
	case SNDRV_CTL_ELEM_TYPE_ENUMERATED:
		for (i = 0; i < info->count; i++) {
			ret = sanity_check_int_value(card, control, info, i,
						     print_error);
			if (ret < 0)
				return ret;
		}
		break;
	default:
		break;
	}

	return 0;
}