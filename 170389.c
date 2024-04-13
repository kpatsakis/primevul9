static void __bpf_prog_put_rcu(struct rcu_head *rcu)
{
	struct bpf_prog_aux *aux = container_of(rcu, struct bpf_prog_aux, rcu);

	kvfree(aux->func_info);
	kfree(aux->func_info_aux);
	free_uid(aux->user);
	security_bpf_prog_free(aux);
	bpf_prog_free(aux->prog);
}