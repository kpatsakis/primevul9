int capi20_manufacturer(unsigned long cmd, void __user *data)
{
	struct capi_ctr *ctr;
	int retval;

	switch (cmd) {
	case KCAPI_CMD_TRACE:
	{
		kcapi_flagdef fdef;

		if (copy_from_user(&fdef, data, sizeof(kcapi_flagdef)))
			return -EFAULT;

		mutex_lock(&capi_controller_lock);

		ctr = get_capi_ctr_by_nr(fdef.contr);
		if (ctr) {
			ctr->traceflag = fdef.flag;
			printk(KERN_INFO "kcapi: contr [%03d] set trace=%d\n",
			       ctr->cnr, ctr->traceflag);
			retval = 0;
		} else
			retval = -ESRCH;

		mutex_unlock(&capi_controller_lock);

		return retval;
	}

	default:
		printk(KERN_ERR "kcapi: manufacturer command %lu unknown.\n",
		       cmd);
		break;

	}
	return -EINVAL;
}