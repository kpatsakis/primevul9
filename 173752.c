static void hidpp_ff_destroy(struct ff_device *ff)
{
	struct hidpp_ff_private_data *data = ff->private;

	kfree(data->effect_ids);
}