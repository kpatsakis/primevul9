static int __maybe_unused i740fb_suspend(struct device *dev)
{
	struct fb_info *info = dev_get_drvdata(dev);
	struct i740fb_par *par = info->par;

	console_lock();
	mutex_lock(&(par->open_lock));

	/* do nothing if framebuffer is not active */
	if (par->ref_count == 0) {
		mutex_unlock(&(par->open_lock));
		console_unlock();
		return 0;
	}

	fb_set_suspend(info, 1);

	mutex_unlock(&(par->open_lock));
	console_unlock();

	return 0;
}