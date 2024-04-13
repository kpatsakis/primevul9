bool is_file_shm_hugepages(struct file *file)
{
	return file->f_op == &shm_file_operations_huge;
}