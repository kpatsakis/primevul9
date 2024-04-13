callout_name_entry(CalloutNameEntry** rentry, OnigEncoding enc,
                   int is_not_single, UChar* name, UChar* name_end)
{
  int r;
  CalloutNameEntry* e;
  CalloutNameTable* t = GlobalCalloutNameTable;

  *rentry = 0;
  if (name_end - name <= 0)
    return ONIGERR_INVALID_CALLOUT_NAME;

  e = callout_name_find(enc, is_not_single, name, name_end);
  if (IS_NULL(e)) {
#ifdef USE_ST_LIBRARY
    if (IS_NULL(t)) {
      t = onig_st_init_callout_name_table_with_size(INIT_NAMES_ALLOC_NUM);
      GlobalCalloutNameTable = t;
    }
    e = (CalloutNameEntry* )xmalloc(sizeof(CalloutNameEntry));
    CHECK_NULL_RETURN_MEMERR(e);

    e->name = onigenc_strdup(enc, name, name_end);
    if (IS_NULL(e->name)) {
      xfree(e);  return ONIGERR_MEMORY;
    }

    r = st_insert_callout_name_table(t, enc, is_not_single,
                                     e->name, (e->name + (name_end - name)),
                                     (HashDataType )e);
    if (r < 0) return r;

#else

    int alloc;

    if (IS_NULL(t)) {
      alloc = INIT_NAMES_ALLOC_NUM;
      t = (CalloutNameTable* )xmalloc(sizeof(CalloutNameTable));
      CHECK_NULL_RETURN_MEMERR(t);
      t->e     = NULL;
      t->alloc = 0;
      t->num   = 0;

      t->e = (CalloutNameEntry* )xmalloc(sizeof(CalloutNameEntry) * alloc);
      if (IS_NULL(t->e)) {
        xfree(t);
        return ONIGERR_MEMORY;
      }
      t->alloc = alloc;
      GlobalCalloutNameTable = t;
      goto clear;
    }
    else if (t->num == t->alloc) {
      int i;

      alloc = t->alloc * 2;
      t->e = (CalloutNameEntry* )xrealloc(t->e, sizeof(CalloutNameEntry) * alloc);
      CHECK_NULL_RETURN_MEMERR(t->e);
      t->alloc = alloc;

    clear:
      for (i = t->num; i < t->alloc; i++) {
        t->e[i].name       = NULL;
        t->e[i].name_len   = 0;
        t->e[i].id         = 0;
      }
    }
    e = &(t->e[t->num]);
    t->num++;
    e->name = onigenc_strdup(enc, name, name_end);
    if (IS_NULL(e->name)) return ONIGERR_MEMORY;
#endif

    CalloutNameIDCounter++;
    e->id = CalloutNameIDCounter;
    e->name_len = (int )(name_end - name);
  }

  *rentry = e;
  return e->id;
}