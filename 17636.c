static int read_user_tlv(struct snd_kcontrol *kctl, unsigned int __user *buf,
			 unsigned int size)
{
	struct user_element *ue = kctl->private_data;

	if (ue->tlv_data_size == 0 || ue->tlv_data == NULL)
		return -ENXIO;

	if (size < ue->tlv_data_size)
		return -ENOSPC;

	if (copy_to_user(buf, ue->tlv_data, ue->tlv_data_size))
		return -EFAULT;

	return 0;
}