mono_icall_init (void)
{
	int i = 0;

	/* check that tables are sorted: disable in release */
	if (TRUE) {
		int j;
		const char *prev_class = NULL;
		const char *prev_method;
		
		for (i = 0; i < Icall_type_num; ++i) {
			const IcallTypeDesc *desc;
			int num_icalls;
			prev_method = NULL;
			if (prev_class && strcmp (prev_class, icall_type_name_get (i)) >= 0)
				g_print ("class %s should come before class %s\n", icall_type_name_get (i), prev_class);
			prev_class = icall_type_name_get (i);
			desc = &icall_type_descs [i];
			num_icalls = icall_desc_num_icalls (desc);
			/*g_print ("class %s has %d icalls starting at %d\n", prev_class, num_icalls, desc->first_icall);*/
			for (j = 0; j < num_icalls; ++j) {
				const char *methodn = icall_name_get (desc->first_icall + j);
				if (prev_method && strcmp (prev_method, methodn) >= 0)
					g_print ("method %s should come before method %s\n", methodn, prev_method);
				prev_method = methodn;
			}
		}
	}

	icall_hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
}