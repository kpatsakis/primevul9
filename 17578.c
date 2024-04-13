static size_t compute_user_elem_size(size_t size, unsigned int count)
{
	return sizeof(struct user_element) + size * count;
}