void ipc_sem_free_info(struct sem_data *semds)
{
	while (semds) {
		struct sem_data *next = semds->next;
		free(semds->elements);
		free(semds);
		semds = next;
	}
}