static int bpf_raw_tracepoint_open(const union bpf_attr *attr)
{
	struct bpf_link_primer link_primer;
	struct bpf_raw_tp_link *link;
	struct bpf_raw_event_map *btp;
	struct bpf_prog *prog;
	const char *tp_name;
	char buf[128];
	int err;

	if (CHECK_ATTR(BPF_RAW_TRACEPOINT_OPEN))
		return -EINVAL;

	prog = bpf_prog_get(attr->raw_tracepoint.prog_fd);
	if (IS_ERR(prog))
		return PTR_ERR(prog);

	switch (prog->type) {
	case BPF_PROG_TYPE_TRACING:
	case BPF_PROG_TYPE_EXT:
	case BPF_PROG_TYPE_LSM:
		if (attr->raw_tracepoint.name) {
			/* The attach point for this category of programs
			 * should be specified via btf_id during program load.
			 */
			err = -EINVAL;
			goto out_put_prog;
		}
		if (prog->type == BPF_PROG_TYPE_TRACING &&
		    prog->expected_attach_type == BPF_TRACE_RAW_TP) {
			tp_name = prog->aux->attach_func_name;
			break;
		}
		err = bpf_tracing_prog_attach(prog, 0, 0);
		if (err >= 0)
			return err;
		goto out_put_prog;
	case BPF_PROG_TYPE_RAW_TRACEPOINT:
	case BPF_PROG_TYPE_RAW_TRACEPOINT_WRITABLE:
		if (strncpy_from_user(buf,
				      u64_to_user_ptr(attr->raw_tracepoint.name),
				      sizeof(buf) - 1) < 0) {
			err = -EFAULT;
			goto out_put_prog;
		}
		buf[sizeof(buf) - 1] = 0;
		tp_name = buf;
		break;
	default:
		err = -EINVAL;
		goto out_put_prog;
	}

	btp = bpf_get_raw_tracepoint(tp_name);
	if (!btp) {
		err = -ENOENT;
		goto out_put_prog;
	}

	link = kzalloc(sizeof(*link), GFP_USER);
	if (!link) {
		err = -ENOMEM;
		goto out_put_btp;
	}
	bpf_link_init(&link->link, BPF_LINK_TYPE_RAW_TRACEPOINT,
		      &bpf_raw_tp_link_lops, prog);
	link->btp = btp;

	err = bpf_link_prime(&link->link, &link_primer);
	if (err) {
		kfree(link);
		goto out_put_btp;
	}

	err = bpf_probe_register(link->btp, prog);
	if (err) {
		bpf_link_cleanup(&link_primer);
		goto out_put_btp;
	}

	return bpf_link_settle(&link_primer);

out_put_btp:
	bpf_put_raw_tracepoint(btp);
out_put_prog:
	bpf_prog_put(prog);
	return err;
}