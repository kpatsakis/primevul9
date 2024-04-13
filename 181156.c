void ipc_shm_free_info(struct shm_data *shmds)
{
	while (shmds) {
		struct shm_data *next = shmds->next;
		free(shmds);
		shmds = next;
	}
}