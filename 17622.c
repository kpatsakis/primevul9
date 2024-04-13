static int snd_ctl_elem_write(struct snd_card *card, struct snd_ctl_file *file,
			      struct snd_ctl_elem_value *control)
{
	struct snd_kcontrol *kctl;
	struct snd_kcontrol_volatile *vd;
	unsigned int index_offset;
	int result;

	down_write(&card->controls_rwsem);
	kctl = snd_ctl_find_id(card, &control->id);
	if (kctl == NULL) {
		up_write(&card->controls_rwsem);
		return -ENOENT;
	}

	index_offset = snd_ctl_get_ioff(kctl, &control->id);
	vd = &kctl->vd[index_offset];
	if (!(vd->access & SNDRV_CTL_ELEM_ACCESS_WRITE) || kctl->put == NULL ||
	    (file && vd->owner && vd->owner != file)) {
		up_write(&card->controls_rwsem);
		return -EPERM;
	}

	snd_ctl_build_ioff(&control->id, kctl, index_offset);
	result = snd_power_ref_and_wait(card);
	/* validate input values */
	if (IS_ENABLED(CONFIG_SND_CTL_INPUT_VALIDATION) && !result) {
		struct snd_ctl_elem_info info;

		memset(&info, 0, sizeof(info));
		info.id = control->id;
		result = __snd_ctl_elem_info(card, kctl, &info, NULL);
		if (!result)
			result = sanity_check_input_values(card, control, &info,
							   false);
	}
	if (!result)
		result = kctl->put(kctl, control);
	snd_power_unref(card);
	if (result < 0) {
		up_write(&card->controls_rwsem);
		return result;
	}

	if (result > 0) {
		downgrade_write(&card->controls_rwsem);
		snd_ctl_notify_one(card, SNDRV_CTL_EVENT_MASK_VALUE, kctl, index_offset);
		up_read(&card->controls_rwsem);
	} else {
		up_write(&card->controls_rwsem);
	}

	return 0;
}