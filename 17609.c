static int snd_ctl_elem_add(struct snd_ctl_file *file,
			    struct snd_ctl_elem_info *info, int replace)
{
	struct snd_card *card = file->card;
	struct snd_kcontrol *kctl;
	unsigned int count;
	unsigned int access;
	long private_size;
	size_t alloc_size;
	struct user_element *ue;
	unsigned int offset;
	int err;

	if (!*info->id.name)
		return -EINVAL;
	if (strnlen(info->id.name, sizeof(info->id.name)) >= sizeof(info->id.name))
		return -EINVAL;

	/* Delete a control to replace them if needed. */
	if (replace) {
		info->id.numid = 0;
		err = snd_ctl_remove_user_ctl(file, &info->id);
		if (err)
			return err;
	}

	/* Check the number of elements for this userspace control. */
	count = info->owner;
	if (count == 0)
		count = 1;

	/* Arrange access permissions if needed. */
	access = info->access;
	if (access == 0)
		access = SNDRV_CTL_ELEM_ACCESS_READWRITE;
	access &= (SNDRV_CTL_ELEM_ACCESS_READWRITE |
		   SNDRV_CTL_ELEM_ACCESS_INACTIVE |
		   SNDRV_CTL_ELEM_ACCESS_TLV_WRITE);

	/* In initial state, nothing is available as TLV container. */
	if (access & SNDRV_CTL_ELEM_ACCESS_TLV_WRITE)
		access |= SNDRV_CTL_ELEM_ACCESS_TLV_CALLBACK;
	access |= SNDRV_CTL_ELEM_ACCESS_USER;

	/*
	 * Check information and calculate the size of data specific to
	 * this userspace control.
	 */
	/* pass NULL to card for suppressing error messages */
	err = snd_ctl_check_elem_info(NULL, info);
	if (err < 0)
		return err;
	/* user-space control doesn't allow zero-size data */
	if (info->count < 1)
		return -EINVAL;
	private_size = value_sizes[info->type] * info->count;
	alloc_size = compute_user_elem_size(private_size, count);

	down_write(&card->controls_rwsem);
	if (check_user_elem_overflow(card, alloc_size)) {
		err = -ENOMEM;
		goto unlock;
	}

	/*
	 * Keep memory object for this userspace control. After passing this
	 * code block, the instance should be freed by snd_ctl_free_one().
	 *
	 * Note that these elements in this control are locked.
	 */
	err = snd_ctl_new(&kctl, count, access, file);
	if (err < 0)
		goto unlock;
	memcpy(&kctl->id, &info->id, sizeof(kctl->id));
	ue = kzalloc(alloc_size, GFP_KERNEL);
	if (!ue) {
		kfree(kctl);
		err = -ENOMEM;
		goto unlock;
	}
	kctl->private_data = ue;
	kctl->private_free = snd_ctl_elem_user_free;

	// increment the allocated size; decremented again at private_free.
	card->user_ctl_alloc_size += alloc_size;

	/* Set private data for this userspace control. */
	ue->card = card;
	ue->info = *info;
	ue->info.access = 0;
	ue->elem_data = (char *)ue + sizeof(*ue);
	ue->elem_data_size = private_size;
	if (ue->info.type == SNDRV_CTL_ELEM_TYPE_ENUMERATED) {
		err = snd_ctl_elem_init_enum_names(ue);
		if (err < 0) {
			snd_ctl_free_one(kctl);
			goto unlock;
		}
	}

	/* Set callback functions. */
	if (info->type == SNDRV_CTL_ELEM_TYPE_ENUMERATED)
		kctl->info = snd_ctl_elem_user_enum_info;
	else
		kctl->info = snd_ctl_elem_user_info;
	if (access & SNDRV_CTL_ELEM_ACCESS_READ)
		kctl->get = snd_ctl_elem_user_get;
	if (access & SNDRV_CTL_ELEM_ACCESS_WRITE)
		kctl->put = snd_ctl_elem_user_put;
	if (access & SNDRV_CTL_ELEM_ACCESS_TLV_WRITE)
		kctl->tlv.c = snd_ctl_elem_user_tlv;

	/* This function manage to free the instance on failure. */
	err = __snd_ctl_add_replace(card, kctl, CTL_ADD_EXCLUSIVE);
	if (err < 0) {
		snd_ctl_free_one(kctl);
		goto unlock;
	}
	offset = snd_ctl_get_ioff(kctl, &info->id);
	snd_ctl_build_ioff(&info->id, kctl, offset);
	/*
	 * Here we cannot fill any field for the number of elements added by
	 * this operation because there're no specific fields. The usage of
	 * 'owner' field for this purpose may cause any bugs to userspace
	 * applications because the field originally means PID of a process
	 * which locks the element.
	 */
 unlock:
	up_write(&card->controls_rwsem);
	return err;
}