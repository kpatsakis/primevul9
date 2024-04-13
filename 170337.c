SYSCALL_DEFINE3(bpf, int, cmd, union bpf_attr __user *, uattr, unsigned int, size)
{
	union bpf_attr attr;
	int err;

	if (sysctl_unprivileged_bpf_disabled && !bpf_capable())
		return -EPERM;

	err = bpf_check_uarg_tail_zero(uattr, sizeof(attr), size);
	if (err)
		return err;
	size = min_t(u32, size, sizeof(attr));

	/* copy attributes from user space, may be less than sizeof(bpf_attr) */
	memset(&attr, 0, sizeof(attr));
	if (copy_from_user(&attr, uattr, size) != 0)
		return -EFAULT;

	err = security_bpf(cmd, &attr, size);
	if (err < 0)
		return err;

	switch (cmd) {
	case BPF_MAP_CREATE:
		err = map_create(&attr);
		break;
	case BPF_MAP_LOOKUP_ELEM:
		err = map_lookup_elem(&attr);
		break;
	case BPF_MAP_UPDATE_ELEM:
		err = map_update_elem(&attr);
		break;
	case BPF_MAP_DELETE_ELEM:
		err = map_delete_elem(&attr);
		break;
	case BPF_MAP_GET_NEXT_KEY:
		err = map_get_next_key(&attr);
		break;
	case BPF_MAP_FREEZE:
		err = map_freeze(&attr);
		break;
	case BPF_PROG_LOAD:
		err = bpf_prog_load(&attr, uattr);
		break;
	case BPF_OBJ_PIN:
		err = bpf_obj_pin(&attr);
		break;
	case BPF_OBJ_GET:
		err = bpf_obj_get(&attr);
		break;
	case BPF_PROG_ATTACH:
		err = bpf_prog_attach(&attr);
		break;
	case BPF_PROG_DETACH:
		err = bpf_prog_detach(&attr);
		break;
	case BPF_PROG_QUERY:
		err = bpf_prog_query(&attr, uattr);
		break;
	case BPF_PROG_TEST_RUN:
		err = bpf_prog_test_run(&attr, uattr);
		break;
	case BPF_PROG_GET_NEXT_ID:
		err = bpf_obj_get_next_id(&attr, uattr,
					  &prog_idr, &prog_idr_lock);
		break;
	case BPF_MAP_GET_NEXT_ID:
		err = bpf_obj_get_next_id(&attr, uattr,
					  &map_idr, &map_idr_lock);
		break;
	case BPF_BTF_GET_NEXT_ID:
		err = bpf_obj_get_next_id(&attr, uattr,
					  &btf_idr, &btf_idr_lock);
		break;
	case BPF_PROG_GET_FD_BY_ID:
		err = bpf_prog_get_fd_by_id(&attr);
		break;
	case BPF_MAP_GET_FD_BY_ID:
		err = bpf_map_get_fd_by_id(&attr);
		break;
	case BPF_OBJ_GET_INFO_BY_FD:
		err = bpf_obj_get_info_by_fd(&attr, uattr);
		break;
	case BPF_RAW_TRACEPOINT_OPEN:
		err = bpf_raw_tracepoint_open(&attr);
		break;
	case BPF_BTF_LOAD:
		err = bpf_btf_load(&attr);
		break;
	case BPF_BTF_GET_FD_BY_ID:
		err = bpf_btf_get_fd_by_id(&attr);
		break;
	case BPF_TASK_FD_QUERY:
		err = bpf_task_fd_query(&attr, uattr);
		break;
	case BPF_MAP_LOOKUP_AND_DELETE_ELEM:
		err = map_lookup_and_delete_elem(&attr);
		break;
	case BPF_MAP_LOOKUP_BATCH:
		err = bpf_map_do_batch(&attr, uattr, BPF_MAP_LOOKUP_BATCH);
		break;
	case BPF_MAP_LOOKUP_AND_DELETE_BATCH:
		err = bpf_map_do_batch(&attr, uattr,
				       BPF_MAP_LOOKUP_AND_DELETE_BATCH);
		break;
	case BPF_MAP_UPDATE_BATCH:
		err = bpf_map_do_batch(&attr, uattr, BPF_MAP_UPDATE_BATCH);
		break;
	case BPF_MAP_DELETE_BATCH:
		err = bpf_map_do_batch(&attr, uattr, BPF_MAP_DELETE_BATCH);
		break;
	case BPF_LINK_CREATE:
		err = link_create(&attr);
		break;
	case BPF_LINK_UPDATE:
		err = link_update(&attr);
		break;
	case BPF_LINK_GET_FD_BY_ID:
		err = bpf_link_get_fd_by_id(&attr);
		break;
	case BPF_LINK_GET_NEXT_ID:
		err = bpf_obj_get_next_id(&attr, uattr,
					  &link_idr, &link_idr_lock);
		break;
	case BPF_ENABLE_STATS:
		err = bpf_enable_stats(&attr);
		break;
	case BPF_ITER_CREATE:
		err = bpf_iter_create(&attr);
		break;
	case BPF_LINK_DETACH:
		err = link_detach(&attr);
		break;
	case BPF_PROG_BIND_MAP:
		err = bpf_prog_bind_map(&attr);
		break;
	default:
		err = -EINVAL;
		break;
	}

	return err;
}