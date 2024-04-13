static int hidpp_ff_queue_work(struct hidpp_ff_private_data *data, int effect_id, u8 command, u8 *params, u8 size)
{
	struct hidpp_ff_work_data *wd = kzalloc(sizeof(*wd), GFP_KERNEL);
	int s;

	if (!wd)
		return -ENOMEM;

	INIT_WORK(&wd->work, hidpp_ff_work_handler);

	wd->data = data;
	wd->effect_id = effect_id;
	wd->command = command;
	wd->size = size;
	memcpy(wd->params, params, size);

	atomic_inc(&data->workqueue_size);
	queue_work(data->wq, &wd->work);

	/* warn about excessive queue size */
	s = atomic_read(&data->workqueue_size);
	if (s >= 20 && s % 20 == 0)
		hid_warn(data->hidpp->hid_dev, "Force feedback command queue contains %d commands, causing substantial delays!", s);

	return 0;
}