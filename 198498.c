static const cac_object_t *cac_find_obj_by_id(unsigned short object_id)
{
	int i;

	for (i = 0; i < cac_object_count; i++) {
		if (cac_objects[i].fd == object_id) {
			return &cac_objects[i];
		}
	}
	return NULL;
}