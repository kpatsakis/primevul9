check_pointer_and_size(Pe *pe, void *ptr, size_t size)
{
	void *map = NULL;
	size_t map_size = 0;

	map = pe_rawfile(pe, &map_size);
	if (!map || map_size < 1)
		return 0;

	if ((uintptr_t)ptr < (uintptr_t)map)
		return 0;

	if ((uintptr_t)ptr + size > (uintptr_t)map + map_size)
		return 0;

	if (ptr <= map && size >= map_size)
		return 0;

	return 1;
}