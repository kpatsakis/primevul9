static void mdesc_get_node_ops(const char *node_name,
			       mdesc_node_info_get_f *get_info_f,
			       mdesc_node_info_rel_f *rel_info_f,
			       mdesc_node_match_f *match_f)
{
	int i;

	if (get_info_f)
		*get_info_f = NULL;

	if (rel_info_f)
		*rel_info_f = NULL;

	if (match_f)
		*match_f = NULL;

	if (!node_name)
		return;

	for (i = 0; md_node_ops_table[i].name != NULL; i++) {
		if (strcmp(md_node_ops_table[i].name, node_name) == 0) {
			if (get_info_f)
				*get_info_f = md_node_ops_table[i].get_info;

			if (rel_info_f)
				*rel_info_f = md_node_ops_table[i].rel_info;

			if (match_f)
				*match_f = md_node_ops_table[i].node_match;

			break;
		}
	}
}