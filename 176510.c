static int i740fb_open(struct fb_info *info, int user)
{
	struct i740fb_par *par = info->par;

	mutex_lock(&(par->open_lock));
	par->ref_count++;
	mutex_unlock(&(par->open_lock));

	return 0;
}