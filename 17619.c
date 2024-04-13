void snd_ctl_notify(struct snd_card *card, unsigned int mask,
		    struct snd_ctl_elem_id *id)
{
	unsigned long flags;
	struct snd_ctl_file *ctl;
	struct snd_kctl_event *ev;

	if (snd_BUG_ON(!card || !id))
		return;
	if (card->shutdown)
		return;
	read_lock_irqsave(&card->ctl_files_rwlock, flags);
#if IS_ENABLED(CONFIG_SND_MIXER_OSS)
	card->mixer_oss_change_count++;
#endif
	list_for_each_entry(ctl, &card->ctl_files, list) {
		if (!ctl->subscribed)
			continue;
		spin_lock(&ctl->read_lock);
		list_for_each_entry(ev, &ctl->events, list) {
			if (ev->id.numid == id->numid) {
				ev->mask |= mask;
				goto _found;
			}
		}
		ev = kzalloc(sizeof(*ev), GFP_ATOMIC);
		if (ev) {
			ev->id = *id;
			ev->mask = mask;
			list_add_tail(&ev->list, &ctl->events);
		} else {
			dev_err(card->dev, "No memory available to allocate event\n");
		}
	_found:
		wake_up(&ctl->change_sleep);
		spin_unlock(&ctl->read_lock);
		snd_kill_fasync(ctl->fasync, SIGIO, POLL_IN);
	}
	read_unlock_irqrestore(&card->ctl_files_rwlock, flags);
}