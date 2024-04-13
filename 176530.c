static int i740fb_release(struct fb_info *info, int user)
{
	struct i740fb_par *par = info->par;

	mutex_lock(&(par->open_lock));
	if (par->ref_count == 0) {
		fb_err(info, "release called with zero refcount\n");
		mutex_unlock(&(par->open_lock));
		return -EINVAL;
	}

	par->ref_count--;
	mutex_unlock(&(par->open_lock));

	return 0;
}