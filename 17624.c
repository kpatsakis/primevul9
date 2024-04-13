static long snd_ctl_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct snd_ctl_file *ctl;
	struct snd_card *card;
	struct snd_kctl_ioctl *p;
	void __user *argp = (void __user *)arg;
	int __user *ip = argp;
	int err;

	ctl = file->private_data;
	card = ctl->card;
	if (snd_BUG_ON(!card))
		return -ENXIO;
	switch (cmd) {
	case SNDRV_CTL_IOCTL_PVERSION:
		return put_user(SNDRV_CTL_VERSION, ip) ? -EFAULT : 0;
	case SNDRV_CTL_IOCTL_CARD_INFO:
		return snd_ctl_card_info(card, ctl, cmd, argp);
	case SNDRV_CTL_IOCTL_ELEM_LIST:
		return snd_ctl_elem_list_user(card, argp);
	case SNDRV_CTL_IOCTL_ELEM_INFO:
		return snd_ctl_elem_info_user(ctl, argp);
	case SNDRV_CTL_IOCTL_ELEM_READ:
		return snd_ctl_elem_read_user(card, argp);
	case SNDRV_CTL_IOCTL_ELEM_WRITE:
		return snd_ctl_elem_write_user(ctl, argp);
	case SNDRV_CTL_IOCTL_ELEM_LOCK:
		return snd_ctl_elem_lock(ctl, argp);
	case SNDRV_CTL_IOCTL_ELEM_UNLOCK:
		return snd_ctl_elem_unlock(ctl, argp);
	case SNDRV_CTL_IOCTL_ELEM_ADD:
		return snd_ctl_elem_add_user(ctl, argp, 0);
	case SNDRV_CTL_IOCTL_ELEM_REPLACE:
		return snd_ctl_elem_add_user(ctl, argp, 1);
	case SNDRV_CTL_IOCTL_ELEM_REMOVE:
		return snd_ctl_elem_remove(ctl, argp);
	case SNDRV_CTL_IOCTL_SUBSCRIBE_EVENTS:
		return snd_ctl_subscribe_events(ctl, ip);
	case SNDRV_CTL_IOCTL_TLV_READ:
		down_read(&ctl->card->controls_rwsem);
		err = snd_ctl_tlv_ioctl(ctl, argp, SNDRV_CTL_TLV_OP_READ);
		up_read(&ctl->card->controls_rwsem);
		return err;
	case SNDRV_CTL_IOCTL_TLV_WRITE:
		down_write(&ctl->card->controls_rwsem);
		err = snd_ctl_tlv_ioctl(ctl, argp, SNDRV_CTL_TLV_OP_WRITE);
		up_write(&ctl->card->controls_rwsem);
		return err;
	case SNDRV_CTL_IOCTL_TLV_COMMAND:
		down_write(&ctl->card->controls_rwsem);
		err = snd_ctl_tlv_ioctl(ctl, argp, SNDRV_CTL_TLV_OP_CMD);
		up_write(&ctl->card->controls_rwsem);
		return err;
	case SNDRV_CTL_IOCTL_POWER:
		return -ENOPROTOOPT;
	case SNDRV_CTL_IOCTL_POWER_STATE:
		return put_user(SNDRV_CTL_POWER_D0, ip) ? -EFAULT : 0;
	}
	down_read(&snd_ioctl_rwsem);
	list_for_each_entry(p, &snd_control_ioctls, list) {
		err = p->fioctl(card, ctl, cmd, arg);
		if (err != -ENOIOCTLCMD) {
			up_read(&snd_ioctl_rwsem);
			return err;
		}
	}
	up_read(&snd_ioctl_rwsem);
	dev_dbg(card->dev, "unknown ioctl = 0x%x\n", cmd);
	return -ENOTTY;
}