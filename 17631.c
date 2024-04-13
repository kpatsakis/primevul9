static int read_tlv_buf(struct snd_kcontrol *kctl, struct snd_ctl_elem_id *id,
			unsigned int __user *buf, unsigned int size)
{
	struct snd_kcontrol_volatile *vd = &kctl->vd[snd_ctl_get_ioff(kctl, id)];
	unsigned int len;

	if (!(vd->access & SNDRV_CTL_ELEM_ACCESS_TLV_READ))
		return -ENXIO;

	if (kctl->tlv.p == NULL)
		return -ENXIO;

	len = sizeof(unsigned int) * 2 + kctl->tlv.p[1];
	if (size < len)
		return -ENOMEM;

	if (copy_to_user(buf, kctl->tlv.p, len))
		return -EFAULT;

	return 0;
}