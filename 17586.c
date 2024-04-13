static int replace_user_tlv(struct snd_kcontrol *kctl, unsigned int __user *buf,
			    unsigned int size)
{
	struct user_element *ue = kctl->private_data;
	unsigned int *container;
	unsigned int mask = 0;
	int i;
	int change;

	if (size > 1024 * 128)	/* sane value */
		return -EINVAL;

	// does the TLV size change cause overflow?
	if (check_user_elem_overflow(ue->card, (ssize_t)(size - ue->tlv_data_size)))
		return -ENOMEM;

	container = vmemdup_user(buf, size);
	if (IS_ERR(container))
		return PTR_ERR(container);

	change = ue->tlv_data_size != size;
	if (!change)
		change = memcmp(ue->tlv_data, container, size) != 0;
	if (!change) {
		kvfree(container);
		return 0;
	}

	if (ue->tlv_data == NULL) {
		/* Now TLV data is available. */
		for (i = 0; i < kctl->count; ++i)
			kctl->vd[i].access |= SNDRV_CTL_ELEM_ACCESS_TLV_READ;
		mask = SNDRV_CTL_EVENT_MASK_INFO;
	} else {
		ue->card->user_ctl_alloc_size -= ue->tlv_data_size;
		ue->tlv_data_size = 0;
		kvfree(ue->tlv_data);
	}

	ue->tlv_data = container;
	ue->tlv_data_size = size;
	// decremented at private_free.
	ue->card->user_ctl_alloc_size += size;

	mask |= SNDRV_CTL_EVENT_MASK_TLV;
	for (i = 0; i < kctl->count; ++i)
		snd_ctl_notify_one(ue->card, mask, kctl, i);

	return change;
}