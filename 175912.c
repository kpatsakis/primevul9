static void delete_mapping(void *data)
{
	soapMappingPtr map = (soapMappingPtr)data;

	if (map->to_xml) {
		zval_ptr_dtor(&map->to_xml);
	}
	if (map->to_zval) {
		zval_ptr_dtor(&map->to_zval);
	}
	efree(map);
}