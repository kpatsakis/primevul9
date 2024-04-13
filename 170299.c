void bpf_link_cleanup(struct bpf_link_primer *primer)
{
	primer->link->prog = NULL;
	bpf_link_free_id(primer->id);
	fput(primer->file);
	put_unused_fd(primer->fd);
}