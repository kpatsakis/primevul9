static void snd_ctl_elem_user_free(struct snd_kcontrol *kcontrol)
{
	struct user_element *ue = kcontrol->private_data;

	// decrement the allocation size.
	ue->card->user_ctl_alloc_size -= compute_user_elem_size(ue->elem_data_size, kcontrol->count);
	ue->card->user_ctl_alloc_size -= ue->tlv_data_size;
	if (ue->priv_data)
		ue->card->user_ctl_alloc_size -= ue->info.value.enumerated.names_length;

	kvfree(ue->tlv_data);
	kvfree(ue->priv_data);
	kfree(ue);
}