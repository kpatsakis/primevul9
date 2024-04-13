static void add_prefix(smart_str* loc_name, char* key_name)
{
	if( strncmp(key_name , LOC_PRIVATE_TAG , 7) == 0 ){
		smart_str_appendl(loc_name, SEPARATOR , sizeof(SEPARATOR)-1);
		smart_str_appendl(loc_name, PRIVATE_PREFIX , sizeof(PRIVATE_PREFIX)-1);
	}
}