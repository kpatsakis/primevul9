static int snd_ctl_elem_user_info(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_info *uinfo)
{
	struct user_element *ue = kcontrol->private_data;
	unsigned int offset;

	offset = snd_ctl_get_ioff(kcontrol, &uinfo->id);
	*uinfo = ue->info;
	snd_ctl_build_ioff(&uinfo->id, kcontrol, offset);

	return 0;
}