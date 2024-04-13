int snd_ctl_enum_info(struct snd_ctl_elem_info *info, unsigned int channels,
		      unsigned int items, const char *const names[])
{
	info->type = SNDRV_CTL_ELEM_TYPE_ENUMERATED;
	info->count = channels;
	info->value.enumerated.items = items;
	if (!items)
		return 0;
	if (info->value.enumerated.item >= items)
		info->value.enumerated.item = items - 1;
	WARN(strlen(names[info->value.enumerated.item]) >= sizeof(info->value.enumerated.name),
	     "ALSA: too long item name '%s'\n",
	     names[info->value.enumerated.item]);
	strscpy(info->value.enumerated.name,
		names[info->value.enumerated.item],
		sizeof(info->value.enumerated.name));
	return 0;
}