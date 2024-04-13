static bool elem_id_matches(const struct snd_kcontrol *kctl,
			    const struct snd_ctl_elem_id *id)
{
	return kctl->id.iface == id->iface &&
		kctl->id.device == id->device &&
		kctl->id.subdevice == id->subdevice &&
		!strncmp(kctl->id.name, id->name, sizeof(kctl->id.name)) &&
		kctl->id.index <= id->index &&
		kctl->id.index + kctl->count > id->index;
}