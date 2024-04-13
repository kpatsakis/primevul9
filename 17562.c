static int sanity_check_elem_value(struct snd_card *card,
				   const struct snd_ctl_elem_value *control,
				   const struct snd_ctl_elem_info *info,
				   u32 pattern)
{
	size_t offset;
	int ret;
	u32 *p;

	ret = sanity_check_input_values(card, control, info, true);
	if (ret < 0)
		return ret;

	/* check whether the remaining area kept untouched */
	offset = value_sizes[info->type] * info->count;
	offset = DIV_ROUND_UP(offset, sizeof(u32));
	p = (u32 *)control->value.bytes.data + offset;
	for (; offset < sizeof(control->value) / sizeof(u32); offset++, p++) {
		if (*p != pattern) {
			ret = -EINVAL;
			break;
		}
		*p = 0; /* clear the checked area */
	}

	return ret;
}