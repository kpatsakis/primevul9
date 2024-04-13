apprentice_unmap(struct magic_map *map)
{
	if (map == NULL)
		return;
	if (map->p != NULL && map->p != php_magic_database) {
		efree(map->p);
	}
	efree(map);
}