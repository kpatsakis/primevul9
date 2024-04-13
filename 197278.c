void bpf_map_fd_put_ptr(void *ptr)
{
	/* ptr->ops->map_free() has to go through one
	 * rcu grace period by itself.
	 */
	bpf_map_put(ptr);
}