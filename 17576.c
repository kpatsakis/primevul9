static int snd_ctl_elem_list(struct snd_card *card,
			     struct snd_ctl_elem_list *list)
{
	struct snd_kcontrol *kctl;
	struct snd_ctl_elem_id id;
	unsigned int offset, space, jidx;
	int err = 0;

	offset = list->offset;
	space = list->space;

	down_read(&card->controls_rwsem);
	list->count = card->controls_count;
	list->used = 0;
	if (space > 0) {
		list_for_each_entry(kctl, &card->controls, list) {
			if (offset >= kctl->count) {
				offset -= kctl->count;
				continue;
			}
			for (jidx = offset; jidx < kctl->count; jidx++) {
				snd_ctl_build_ioff(&id, kctl, jidx);
				if (copy_to_user(list->pids + list->used, &id,
						 sizeof(id))) {
					err = -EFAULT;
					goto out;
				}
				list->used++;
				if (!--space)
					goto out;
			}
			offset = 0;
		}
	}
 out:
	up_read(&card->controls_rwsem);
	return err;
}