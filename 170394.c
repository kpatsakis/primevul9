static int bpf_task_fd_query(const union bpf_attr *attr,
			     union bpf_attr __user *uattr)
{
	pid_t pid = attr->task_fd_query.pid;
	u32 fd = attr->task_fd_query.fd;
	const struct perf_event *event;
	struct task_struct *task;
	struct file *file;
	int err;

	if (CHECK_ATTR(BPF_TASK_FD_QUERY))
		return -EINVAL;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (attr->task_fd_query.flags != 0)
		return -EINVAL;

	task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
	if (!task)
		return -ENOENT;

	err = 0;
	file = fget_task(task, fd);
	put_task_struct(task);
	if (!file)
		return -EBADF;

	if (file->f_op == &bpf_link_fops) {
		struct bpf_link *link = file->private_data;

		if (link->ops == &bpf_raw_tp_link_lops) {
			struct bpf_raw_tp_link *raw_tp =
				container_of(link, struct bpf_raw_tp_link, link);
			struct bpf_raw_event_map *btp = raw_tp->btp;

			err = bpf_task_fd_query_copy(attr, uattr,
						     raw_tp->link.prog->aux->id,
						     BPF_FD_TYPE_RAW_TRACEPOINT,
						     btp->tp->name, 0, 0);
			goto put_file;
		}
		goto out_not_supp;
	}

	event = perf_get_event(file);
	if (!IS_ERR(event)) {
		u64 probe_offset, probe_addr;
		u32 prog_id, fd_type;
		const char *buf;

		err = bpf_get_perf_event_info(event, &prog_id, &fd_type,
					      &buf, &probe_offset,
					      &probe_addr);
		if (!err)
			err = bpf_task_fd_query_copy(attr, uattr, prog_id,
						     fd_type, buf,
						     probe_offset,
						     probe_addr);
		goto put_file;
	}

out_not_supp:
	err = -ENOTSUPP;
put_file:
	fput(file);
	return err;
}