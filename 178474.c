static int pmcmd_ioctl(u64 cmd, void __user *uptr)
{
	struct acrn_pstate_data *px_data;
	struct acrn_cstate_data *cx_data;
	u64 *pm_info;
	int ret = 0;

	switch (cmd & PMCMD_TYPE_MASK) {
	case ACRN_PMCMD_GET_PX_CNT:
	case ACRN_PMCMD_GET_CX_CNT:
		pm_info = kmalloc(sizeof(u64), GFP_KERNEL);
		if (!pm_info)
			return -ENOMEM;

		ret = hcall_get_cpu_state(cmd, virt_to_phys(pm_info));
		if (ret < 0) {
			kfree(pm_info);
			break;
		}

		if (copy_to_user(uptr, pm_info, sizeof(u64)))
			ret = -EFAULT;
		kfree(pm_info);
		break;
	case ACRN_PMCMD_GET_PX_DATA:
		px_data = kmalloc(sizeof(*px_data), GFP_KERNEL);
		if (!px_data)
			return -ENOMEM;

		ret = hcall_get_cpu_state(cmd, virt_to_phys(px_data));
		if (ret < 0) {
			kfree(px_data);
			break;
		}

		if (copy_to_user(uptr, px_data, sizeof(*px_data)))
			ret = -EFAULT;
		kfree(px_data);
		break;
	case ACRN_PMCMD_GET_CX_DATA:
		cx_data = kmalloc(sizeof(*cx_data), GFP_KERNEL);
		if (!cx_data)
			return -ENOMEM;

		ret = hcall_get_cpu_state(cmd, virt_to_phys(cx_data));
		if (ret < 0) {
			kfree(cx_data);
			break;
		}

		if (copy_to_user(uptr, cx_data, sizeof(*cx_data)))
			ret = -EFAULT;
		kfree(cx_data);
		break;
	default:
		break;
	}

	return ret;
}