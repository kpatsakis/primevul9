static void add_to_kill(struct task_struct *tsk, struct page *p,
		       struct vm_area_struct *vma,
		       struct list_head *to_kill,
		       struct to_kill **tkc)
{
	struct to_kill *tk;

	if (*tkc) {
		tk = *tkc;
		*tkc = NULL;
	} else {
		tk = kmalloc(sizeof(struct to_kill), GFP_ATOMIC);
		if (!tk) {
			pr_err("Memory failure: Out of memory while machine check handling\n");
			return;
		}
	}
	tk->addr = page_address_in_vma(p, vma);
	tk->addr_valid = 1;

	/*
	 * In theory we don't have to kill when the page was
	 * munmaped. But it could be also a mremap. Since that's
	 * likely very rare kill anyways just out of paranoia, but use
	 * a SIGKILL because the error is not contained anymore.
	 */
	if (tk->addr == -EFAULT) {
		pr_info("Memory failure: Unable to find user space address %lx in %s\n",
			page_to_pfn(p), tsk->comm);
		tk->addr_valid = 0;
	}
	get_task_struct(tsk);
	tk->tsk = tsk;
	list_add_tail(&tk->nd, to_kill);
}