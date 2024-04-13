static void get_client_info(struct snd_seq_client *cptr,
			    struct snd_seq_client_info *info)
{
	info->client = cptr->number;

	/* fill the info fields */
	info->type = cptr->type;
	strcpy(info->name, cptr->name);
	info->filter = cptr->filter;
	info->event_lost = cptr->event_lost;
	memcpy(info->event_filter, cptr->event_filter, 32);
	info->num_ports = cptr->num_ports;

	if (cptr->type == USER_CLIENT)
		info->pid = pid_vnr(cptr->data.user.owner);
	else
		info->pid = -1;

	if (cptr->type == KERNEL_CLIENT)
		info->card = cptr->data.kernel.card ? cptr->data.kernel.card->number : -1;
	else
		info->card = -1;

	memset(info->reserved, 0, sizeof(info->reserved));
}