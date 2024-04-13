static int bpf_prog_get_info_by_fd(struct file *file,
				   struct bpf_prog *prog,
				   const union bpf_attr *attr,
				   union bpf_attr __user *uattr)
{
	struct bpf_prog_info __user *uinfo = u64_to_user_ptr(attr->info.info);
	struct bpf_prog_info info;
	u32 info_len = attr->info.info_len;
	struct bpf_prog_stats stats;
	char __user *uinsns;
	u32 ulen;
	int err;

	err = bpf_check_uarg_tail_zero(uinfo, sizeof(info), info_len);
	if (err)
		return err;
	info_len = min_t(u32, sizeof(info), info_len);

	memset(&info, 0, sizeof(info));
	if (copy_from_user(&info, uinfo, info_len))
		return -EFAULT;

	info.type = prog->type;
	info.id = prog->aux->id;
	info.load_time = prog->aux->load_time;
	info.created_by_uid = from_kuid_munged(current_user_ns(),
					       prog->aux->user->uid);
	info.gpl_compatible = prog->gpl_compatible;

	memcpy(info.tag, prog->tag, sizeof(prog->tag));
	memcpy(info.name, prog->aux->name, sizeof(prog->aux->name));

	mutex_lock(&prog->aux->used_maps_mutex);
	ulen = info.nr_map_ids;
	info.nr_map_ids = prog->aux->used_map_cnt;
	ulen = min_t(u32, info.nr_map_ids, ulen);
	if (ulen) {
		u32 __user *user_map_ids = u64_to_user_ptr(info.map_ids);
		u32 i;

		for (i = 0; i < ulen; i++)
			if (put_user(prog->aux->used_maps[i]->id,
				     &user_map_ids[i])) {
				mutex_unlock(&prog->aux->used_maps_mutex);
				return -EFAULT;
			}
	}
	mutex_unlock(&prog->aux->used_maps_mutex);

	err = set_info_rec_size(&info);
	if (err)
		return err;

	bpf_prog_get_stats(prog, &stats);
	info.run_time_ns = stats.nsecs;
	info.run_cnt = stats.cnt;
	info.recursion_misses = stats.misses;

	if (!bpf_capable()) {
		info.jited_prog_len = 0;
		info.xlated_prog_len = 0;
		info.nr_jited_ksyms = 0;
		info.nr_jited_func_lens = 0;
		info.nr_func_info = 0;
		info.nr_line_info = 0;
		info.nr_jited_line_info = 0;
		goto done;
	}

	ulen = info.xlated_prog_len;
	info.xlated_prog_len = bpf_prog_insn_size(prog);
	if (info.xlated_prog_len && ulen) {
		struct bpf_insn *insns_sanitized;
		bool fault;

		if (prog->blinded && !bpf_dump_raw_ok(file->f_cred)) {
			info.xlated_prog_insns = 0;
			goto done;
		}
		insns_sanitized = bpf_insn_prepare_dump(prog, file->f_cred);
		if (!insns_sanitized)
			return -ENOMEM;
		uinsns = u64_to_user_ptr(info.xlated_prog_insns);
		ulen = min_t(u32, info.xlated_prog_len, ulen);
		fault = copy_to_user(uinsns, insns_sanitized, ulen);
		kfree(insns_sanitized);
		if (fault)
			return -EFAULT;
	}

	if (bpf_prog_is_dev_bound(prog->aux)) {
		err = bpf_prog_offload_info_fill(&info, prog);
		if (err)
			return err;
		goto done;
	}

	/* NOTE: the following code is supposed to be skipped for offload.
	 * bpf_prog_offload_info_fill() is the place to fill similar fields
	 * for offload.
	 */
	ulen = info.jited_prog_len;
	if (prog->aux->func_cnt) {
		u32 i;

		info.jited_prog_len = 0;
		for (i = 0; i < prog->aux->func_cnt; i++)
			info.jited_prog_len += prog->aux->func[i]->jited_len;
	} else {
		info.jited_prog_len = prog->jited_len;
	}

	if (info.jited_prog_len && ulen) {
		if (bpf_dump_raw_ok(file->f_cred)) {
			uinsns = u64_to_user_ptr(info.jited_prog_insns);
			ulen = min_t(u32, info.jited_prog_len, ulen);

			/* for multi-function programs, copy the JITed
			 * instructions for all the functions
			 */
			if (prog->aux->func_cnt) {
				u32 len, free, i;
				u8 *img;

				free = ulen;
				for (i = 0; i < prog->aux->func_cnt; i++) {
					len = prog->aux->func[i]->jited_len;
					len = min_t(u32, len, free);
					img = (u8 *) prog->aux->func[i]->bpf_func;
					if (copy_to_user(uinsns, img, len))
						return -EFAULT;
					uinsns += len;
					free -= len;
					if (!free)
						break;
				}
			} else {
				if (copy_to_user(uinsns, prog->bpf_func, ulen))
					return -EFAULT;
			}
		} else {
			info.jited_prog_insns = 0;
		}
	}

	ulen = info.nr_jited_ksyms;
	info.nr_jited_ksyms = prog->aux->func_cnt ? : 1;
	if (ulen) {
		if (bpf_dump_raw_ok(file->f_cred)) {
			unsigned long ksym_addr;
			u64 __user *user_ksyms;
			u32 i;

			/* copy the address of the kernel symbol
			 * corresponding to each function
			 */
			ulen = min_t(u32, info.nr_jited_ksyms, ulen);
			user_ksyms = u64_to_user_ptr(info.jited_ksyms);
			if (prog->aux->func_cnt) {
				for (i = 0; i < ulen; i++) {
					ksym_addr = (unsigned long)
						prog->aux->func[i]->bpf_func;
					if (put_user((u64) ksym_addr,
						     &user_ksyms[i]))
						return -EFAULT;
				}
			} else {
				ksym_addr = (unsigned long) prog->bpf_func;
				if (put_user((u64) ksym_addr, &user_ksyms[0]))
					return -EFAULT;
			}
		} else {
			info.jited_ksyms = 0;
		}
	}

	ulen = info.nr_jited_func_lens;
	info.nr_jited_func_lens = prog->aux->func_cnt ? : 1;
	if (ulen) {
		if (bpf_dump_raw_ok(file->f_cred)) {
			u32 __user *user_lens;
			u32 func_len, i;

			/* copy the JITed image lengths for each function */
			ulen = min_t(u32, info.nr_jited_func_lens, ulen);
			user_lens = u64_to_user_ptr(info.jited_func_lens);
			if (prog->aux->func_cnt) {
				for (i = 0; i < ulen; i++) {
					func_len =
						prog->aux->func[i]->jited_len;
					if (put_user(func_len, &user_lens[i]))
						return -EFAULT;
				}
			} else {
				func_len = prog->jited_len;
				if (put_user(func_len, &user_lens[0]))
					return -EFAULT;
			}
		} else {
			info.jited_func_lens = 0;
		}
	}

	if (prog->aux->btf)
		info.btf_id = btf_obj_id(prog->aux->btf);

	ulen = info.nr_func_info;
	info.nr_func_info = prog->aux->func_info_cnt;
	if (info.nr_func_info && ulen) {
		char __user *user_finfo;

		user_finfo = u64_to_user_ptr(info.func_info);
		ulen = min_t(u32, info.nr_func_info, ulen);
		if (copy_to_user(user_finfo, prog->aux->func_info,
				 info.func_info_rec_size * ulen))
			return -EFAULT;
	}

	ulen = info.nr_line_info;
	info.nr_line_info = prog->aux->nr_linfo;
	if (info.nr_line_info && ulen) {
		__u8 __user *user_linfo;

		user_linfo = u64_to_user_ptr(info.line_info);
		ulen = min_t(u32, info.nr_line_info, ulen);
		if (copy_to_user(user_linfo, prog->aux->linfo,
				 info.line_info_rec_size * ulen))
			return -EFAULT;
	}

	ulen = info.nr_jited_line_info;
	if (prog->aux->jited_linfo)
		info.nr_jited_line_info = prog->aux->nr_linfo;
	else
		info.nr_jited_line_info = 0;
	if (info.nr_jited_line_info && ulen) {
		if (bpf_dump_raw_ok(file->f_cred)) {
			__u64 __user *user_linfo;
			u32 i;

			user_linfo = u64_to_user_ptr(info.jited_line_info);
			ulen = min_t(u32, info.nr_jited_line_info, ulen);
			for (i = 0; i < ulen; i++) {
				if (put_user((__u64)(long)prog->aux->jited_linfo[i],
					     &user_linfo[i]))
					return -EFAULT;
			}
		} else {
			info.jited_line_info = 0;
		}
	}

	ulen = info.nr_prog_tags;
	info.nr_prog_tags = prog->aux->func_cnt ? : 1;
	if (ulen) {
		__u8 __user (*user_prog_tags)[BPF_TAG_SIZE];
		u32 i;

		user_prog_tags = u64_to_user_ptr(info.prog_tags);
		ulen = min_t(u32, info.nr_prog_tags, ulen);
		if (prog->aux->func_cnt) {
			for (i = 0; i < ulen; i++) {
				if (copy_to_user(user_prog_tags[i],
						 prog->aux->func[i]->tag,
						 BPF_TAG_SIZE))
					return -EFAULT;
			}
		} else {
			if (copy_to_user(user_prog_tags[0],
					 prog->tag, BPF_TAG_SIZE))
				return -EFAULT;
		}
	}

done:
	if (copy_to_user(uinfo, &info, info_len) ||
	    put_user(info_len, &uattr->info.info_len))
		return -EFAULT;

	return 0;
}