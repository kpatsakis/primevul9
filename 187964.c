static long ioctl_dtprop(struct fsl_hv_ioctl_prop __user *p, int set)
{
	struct fsl_hv_ioctl_prop param;
	char __user *upath, *upropname;
	void __user *upropval;
	char *path, *propname;
	void *propval;
	int ret = 0;

	/* Get the parameters from the user. */
	if (copy_from_user(&param, p, sizeof(struct fsl_hv_ioctl_prop)))
		return -EFAULT;

	upath = (char __user *)(uintptr_t)param.path;
	upropname = (char __user *)(uintptr_t)param.propname;
	upropval = (void __user *)(uintptr_t)param.propval;

	path = strndup_user(upath, FH_DTPROP_MAX_PATHLEN);
	if (IS_ERR(path))
		return PTR_ERR(path);

	propname = strndup_user(upropname, FH_DTPROP_MAX_PATHLEN);
	if (IS_ERR(propname)) {
		ret = PTR_ERR(propname);
		goto err_free_path;
	}

	if (param.proplen > FH_DTPROP_MAX_PROPLEN) {
		ret = -EINVAL;
		goto err_free_propname;
	}

	propval = kmalloc(param.proplen, GFP_KERNEL);
	if (!propval) {
		ret = -ENOMEM;
		goto err_free_propname;
	}

	if (set) {
		if (copy_from_user(propval, upropval, param.proplen)) {
			ret = -EFAULT;
			goto err_free_propval;
		}

		param.ret = fh_partition_set_dtprop(param.handle,
						    virt_to_phys(path),
						    virt_to_phys(propname),
						    virt_to_phys(propval),
						    param.proplen);
	} else {
		param.ret = fh_partition_get_dtprop(param.handle,
						    virt_to_phys(path),
						    virt_to_phys(propname),
						    virt_to_phys(propval),
						    &param.proplen);

		if (param.ret == 0) {
			if (copy_to_user(upropval, propval, param.proplen) ||
			    put_user(param.proplen, &p->proplen)) {
				ret = -EFAULT;
				goto err_free_propval;
			}
		}
	}

	if (put_user(param.ret, &p->ret))
		ret = -EFAULT;

err_free_propval:
	kfree(propval);
err_free_propname:
	kfree(propname);
err_free_path:
	kfree(path);

	return ret;
}