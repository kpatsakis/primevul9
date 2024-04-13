static Sdb *__core_cmd_anal_fcn_stats (RCore *core, const char *input) {
	bool silentMode = false;
	int statsMode = 0;
	if (*input == '*') {
		silentMode = true;
		input++;
	}
	switch (*input) {
	case '?':
		eprintf ("Usage: afis[ft]\n");
		eprintf (" afis           enumerate unique opcodes in function\n");
		eprintf (" afisa[fo]      enumerate all the meta of all the functions\n");
		eprintf (" afisf          enumerate unique opcode families in function\n");
		eprintf (" afiso          enumerate unique opcode types in function\n");
		eprintf (" afist [query]  list in table format\n");
		return NULL;
	case 'f':
	case 'o':
		statsMode = *input;
		input++;
		break;
	}

	RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, -1);
	if (!fcn) {
		eprintf ("Cannot find any function at 0x%08"PFMT64x"\n", core->offset);
		return NULL;
	}
	Sdb *db = sdb_new0 ();
	RAnalBlock *bb;
	RListIter *iter;
	r_list_foreach (fcn->bbs, iter, bb) {
		int i;
		__updateStats (core, db, bb->addr, statsMode);
		for (i = 0; i< bb->op_pos_size; i++) {
			ut16 op_pos = bb->op_pos[i];
			__updateStats (core, db, bb->addr + op_pos, statsMode);
		}
	}
	if (silentMode) {
		// nothing
	} else if (*input == 't') {
		SdbList *ls = sdb_foreach_list (db, true);
		SdbListIter *it;
		RTable *t = r_table_new ();
		SdbKv *kv;
		RTableColumnType *typeString = r_table_type ("string");
		RTableColumnType *typeNumber = r_table_type ("number");
		r_table_add_column (t, typeString, "name", 0);
		ls_foreach (ls, it, kv) {
			const char *key = sdbkv_key (kv);
			r_table_add_column (t, typeNumber, key, 0);
		}
		RList *items = r_list_newf (free);
		r_list_append (items, fcn->name);
		ls_foreach (ls, it, kv) {
			const char *value = sdbkv_value (kv);
			int nv = (int)r_num_get (NULL, value);
			r_list_append (items, r_str_newf ("%d", nv));
		}
		r_table_add_row_list (t, items);
		r_table_query (t, input + 1);
		char *ts = r_table_tostring (t);
		r_cons_printf ("%s", ts);
		free (ts);
		r_table_free (t);
	} else {
		SdbList *ls = sdb_foreach_list (db, true);
		SdbListIter *it;
		SdbKv *kv;
		ls_foreach (ls, it, kv) {
			const char *key = sdbkv_key(kv);
			const char *value = sdbkv_value(kv);
			r_cons_printf ("%4d %s\n", (int)r_num_get (NULL, value), key);
		}
	}
	return db;
	//sdb_free (db);
}