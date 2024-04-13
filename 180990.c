static int snd_pcm_control_ioctl(struct snd_card *card,
				 struct snd_ctl_file *control,
				 unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE:
		{
			int device;

			if (get_user(device, (int __user *)arg))
				return -EFAULT;
			mutex_lock(&register_mutex);
			device = snd_pcm_next(card, device);
			mutex_unlock(&register_mutex);
			if (put_user(device, (int __user *)arg))
				return -EFAULT;
			return 0;
		}
	case SNDRV_CTL_IOCTL_PCM_INFO:
		{
			struct snd_pcm_info __user *info;
			unsigned int device, subdevice;
			int stream;
			struct snd_pcm *pcm;
			struct snd_pcm_str *pstr;
			struct snd_pcm_substream *substream;
			int err;

			info = (struct snd_pcm_info __user *)arg;
			if (get_user(device, &info->device))
				return -EFAULT;
			if (get_user(stream, &info->stream))
				return -EFAULT;
			if (stream < 0 || stream > 1)
				return -EINVAL;
			if (get_user(subdevice, &info->subdevice))
				return -EFAULT;
			mutex_lock(&register_mutex);
			pcm = snd_pcm_get(card, device);
			if (pcm == NULL) {
				err = -ENXIO;
				goto _error;
			}
			pstr = &pcm->streams[stream];
			if (pstr->substream_count == 0) {
				err = -ENOENT;
				goto _error;
			}
			if (subdevice >= pstr->substream_count) {
				err = -ENXIO;
				goto _error;
			}
			for (substream = pstr->substream; substream;
			     substream = substream->next)
				if (substream->number == (int)subdevice)
					break;
			if (substream == NULL) {
				err = -ENXIO;
				goto _error;
			}
			mutex_lock(&pcm->open_mutex);
			err = snd_pcm_info_user(substream, info);
			mutex_unlock(&pcm->open_mutex);
		_error:
			mutex_unlock(&register_mutex);
			return err;
		}
	case SNDRV_CTL_IOCTL_PCM_PREFER_SUBDEVICE:
		{
			int val;
			
			if (get_user(val, (int __user *)arg))
				return -EFAULT;
			control->preferred_subdevice[SND_CTL_SUBDEV_PCM] = val;
			return 0;
		}
	}
	return -ENOIOCTLCMD;
}