int go7007_snd_remove(struct go7007 *go)
{
	struct go7007_snd *gosnd = go->snd_context;

	snd_card_disconnect(gosnd->card);
	snd_card_free_when_closed(gosnd->card);
	v4l2_device_put(&go->v4l2_dev);
	return 0;
}