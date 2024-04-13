static int call_tlv_handler(struct snd_ctl_file *file, int op_flag,
			    struct snd_kcontrol *kctl,
			    struct snd_ctl_elem_id *id,
			    unsigned int __user *buf, unsigned int size)
{
	static const struct {
		int op;
		int perm;
	} pairs[] = {
		{SNDRV_CTL_TLV_OP_READ,  SNDRV_CTL_ELEM_ACCESS_TLV_READ},
		{SNDRV_CTL_TLV_OP_WRITE, SNDRV_CTL_ELEM_ACCESS_TLV_WRITE},
		{SNDRV_CTL_TLV_OP_CMD,   SNDRV_CTL_ELEM_ACCESS_TLV_COMMAND},
	};
	struct snd_kcontrol_volatile *vd = &kctl->vd[snd_ctl_get_ioff(kctl, id)];
	int i, ret;

	/* Check support of the request for this element. */
	for (i = 0; i < ARRAY_SIZE(pairs); ++i) {
		if (op_flag == pairs[i].op && (vd->access & pairs[i].perm))
			break;
	}
	if (i == ARRAY_SIZE(pairs))
		return -ENXIO;

	if (kctl->tlv.c == NULL)
		return -ENXIO;

	/* Write and command operations are not allowed for locked element. */
	if (op_flag != SNDRV_CTL_TLV_OP_READ &&
	    vd->owner != NULL && vd->owner != file)
		return -EPERM;

	ret = snd_power_ref_and_wait(file->card);
	if (!ret)
		ret = kctl->tlv.c(kctl, op_flag, size, buf);
	snd_power_unref(file->card);
	return ret;
}