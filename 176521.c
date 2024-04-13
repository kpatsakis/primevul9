static int __maybe_unused i740fb_resume(struct device *dev)
{
	struct fb_info *info = dev_get_drvdata(dev);
	struct i740fb_par *par = info->par;

	console_lock();
	mutex_lock(&(par->open_lock));

	if (par->ref_count == 0)
		goto fail;

	i740fb_set_par(info);
	fb_set_suspend(info, 0);

fail:
	mutex_unlock(&(par->open_lock));
	console_unlock();
	return 0;
}