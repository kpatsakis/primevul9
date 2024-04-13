static int go7007_snd_free(struct snd_device *device)
{
	struct go7007 *go = device->device_data;

	kfree(go->snd_context);
	go->snd_context = NULL;
	return 0;
}