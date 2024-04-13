static void __core_cmd_anal_fcn_allstats(RCore *core, const char *input) {
	RAnalFunction *fcn;
	SdbKv *kv;
	RListIter *iter;
	SdbListIter *it;
	RList *dbs = r_list_newf ((RListFree)sdb_free);
	Sdb *d = sdb_new0 ();
	ut64 oseek = core->offset;
	bool isJson = strchr (input, 'j') != NULL;

	char *inp = r_str_newf ("*%s", input);
	r_list_foreach (core->anal->fcns, iter, fcn) {
		r_core_seek (core, fcn->addr, true);
		Sdb *db = __core_cmd_anal_fcn_stats (core, inp);
                sdb_num_set (db, ".addr", fcn->addr, 0);
		r_list_append (dbs, db);
	}
	free (inp);
	Sdb *db;
	r_list_foreach (dbs, iter, db) {
		SdbList *ls = sdb_foreach_list (db, true);
		ls_foreach (ls, it, kv) {
			const char *name = sdbkv_key(kv);
			sdb_add (d, name, "1", 0);
		}
		ls_free (ls);
	}
	RTable *t = r_table_new ();
	SdbList *ls = sdb_foreach_list (d, true);
	RTableColumnType *typeString = r_table_type ("string");
	RTableColumnType *typeNumber = r_table_type ("number");
	r_table_add_column (t, typeString, "name", 0);
	r_table_add_column (t, typeNumber, "addr", 0);
	ls_foreach (ls, it, kv) {
		const char *key = sdbkv_key (kv);
		if (*key == '.') continue;
		r_table_add_column (t, typeNumber, key, 0);
	}
	sdb_free (d);

	r_list_foreach (dbs, iter, db) {
		SdbList *ls = sdb_foreach_list (db, false);
		SdbListIter *it;
		SdbKv *kv;
		char *names[100];
		int i;
		for (i = 0;i<100;i++) {
			names[i] = NULL;
		}
		ls_foreach (ls, it, kv) {
			const char *key = sdbkv_key(kv);
			const char *value = sdbkv_value (kv);
			if (*key == '.') {
				continue;
			}
			int idx = r_table_column_nth (t, key);
			if (idx != -1) {
				ut64 nv = r_num_get (NULL, value);
				names[idx] = r_str_newf ("%d", (int)nv);
			} else {
				eprintf ("Invalid column name (%s) %c", key, 10);
			}
		}
		RList *items = r_list_newf (free);
		ut64 fcnAddr = sdb_num_get (db, ".addr", 0);

		RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, fcnAddr, 0);
		r_list_append (items, fcn?strdup (fcn->name):strdup (""));
		r_list_append (items, fcn?r_str_newf ("0x%08"PFMT64x, fcnAddr): strdup ("0"));
		int cols = r_list_length (t->cols);
		for (i = 2; i < cols; i++) {
			if (names[i]) {
				if (names[i][0] != '.') {
					r_list_append (items, strdup (names[i]));
				}
				R_FREE (names[i]);
			} else {
				r_list_append (items, strdup ("0"));
			}
		}
		r_table_add_row_list (t, items);
	}
	r_table_query (t, (*input)?input + 1: "");
	char *ts = isJson? r_table_tojson(t): r_table_tostring (t);
	r_cons_printf ("%s", ts);
	free (ts);
	r_table_free (t);
	r_core_seek (core, oseek, true);
	r_list_free (dbs);
}