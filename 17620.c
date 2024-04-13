void snd_ctl_free_one(struct snd_kcontrol *kcontrol)
{
	if (kcontrol) {
		if (kcontrol->private_free)
			kcontrol->private_free(kcontrol);
		kfree(kcontrol);
	}
}