static int snd_seq_ioctl_running_mode(struct snd_seq_client *client, void  *arg)
{
	struct snd_seq_running_info *info = arg;
	struct snd_seq_client *cptr;
	int err = 0;

	/* requested client number */
	cptr = snd_seq_client_use_ptr(info->client);
	if (cptr == NULL)
		return -ENOENT;		/* don't change !!! */

#ifdef SNDRV_BIG_ENDIAN
	if (!info->big_endian) {
		err = -EINVAL;
		goto __err;
	}
#else
	if (info->big_endian) {
		err = -EINVAL;
		goto __err;
	}

#endif
	if (info->cpu_mode > sizeof(long)) {
		err = -EINVAL;
		goto __err;
	}
	cptr->convert32 = (info->cpu_mode < sizeof(long));
 __err:
	snd_seq_client_unlock(cptr);
	return err;
}