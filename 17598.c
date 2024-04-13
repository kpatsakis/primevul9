int snd_ctl_request_layer(const char *module_name)
{
	struct snd_ctl_layer_ops *lops;

	if (module_name == NULL)
		return 0;
	down_read(&snd_ctl_layer_rwsem);
	for (lops = snd_ctl_layer; lops; lops = lops->next)
		if (strcmp(lops->module_name, module_name) == 0)
			break;
	up_read(&snd_ctl_layer_rwsem);
	if (lops)
		return 0;
	return request_module(module_name);
}