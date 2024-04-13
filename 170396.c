static int bpf_obj_get_next_id(const union bpf_attr *attr,
			       union bpf_attr __user *uattr,
			       struct idr *idr,
			       spinlock_t *lock)
{
	u32 next_id = attr->start_id;
	int err = 0;

	if (CHECK_ATTR(BPF_OBJ_GET_NEXT_ID) || next_id >= INT_MAX)
		return -EINVAL;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	next_id++;
	spin_lock_bh(lock);
	if (!idr_get_next(idr, &next_id))
		err = -ENOENT;
	spin_unlock_bh(lock);

	if (!err)
		err = put_user(next_id, &uattr->next_id);

	return err;
}