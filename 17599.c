static void fill_remaining_elem_value(struct snd_ctl_elem_value *control,
				      struct snd_ctl_elem_info *info,
				      u32 pattern)
{
	size_t offset = value_sizes[info->type] * info->count;

	offset = DIV_ROUND_UP(offset, sizeof(u32));
	memset32((u32 *)control->value.bytes.data + offset, pattern,
		 sizeof(control->value) / sizeof(u32) - offset);
}