static int snd_ctl_elem_user_tlv(struct snd_kcontrol *kctl, int op_flag,
				 unsigned int size, unsigned int __user *buf)
{
	if (op_flag == SNDRV_CTL_TLV_OP_WRITE)
		return replace_user_tlv(kctl, buf, size);
	else
		return read_user_tlv(kctl, buf, size);
}