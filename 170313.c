static void __bpf_prog_put_noref(struct bpf_prog *prog, bool deferred)
{
	bpf_prog_kallsyms_del_all(prog);
	btf_put(prog->aux->btf);
	bpf_prog_free_linfo(prog);
	if (prog->aux->attach_btf)
		btf_put(prog->aux->attach_btf);

	if (deferred) {
		if (prog->aux->sleepable)
			call_rcu_tasks_trace(&prog->aux->rcu, __bpf_prog_put_rcu);
		else
			call_rcu(&prog->aux->rcu, __bpf_prog_put_rcu);
	} else {
		__bpf_prog_put_rcu(&prog->aux->rcu);
	}
}