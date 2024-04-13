name_add(regex_t* reg, UChar* name, UChar* name_end, int backref, ScanEnv* env)
{
  int r;
  int alloc;
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  if (name_end - name <= 0)
    return ONIGERR_EMPTY_GROUP_NAME;

  e = name_find(reg, name, name_end);
  if (IS_NULL(e)) {
#ifdef USE_ST_LIBRARY
    if (IS_NULL(t)) {
      t = onig_st_init_strend_table_with_size(INIT_NAMES_ALLOC_NUM);
      reg->name_table = (void* )t;
    }
    e = (NameEntry* )xmalloc(sizeof(NameEntry));
    CHECK_NULL_RETURN_MEMERR(e);

    e->name = onigenc_strdup(reg->enc, name, name_end);
    if (IS_NULL(e->name)) {
      xfree(e);  return ONIGERR_MEMORY;
    }
    r = onig_st_insert_strend(t, e->name, (e->name + (name_end - name)),
                              (HashDataType )e);
    if (r < 0) return r;

    e->name_len   = (int )(name_end - name);
    e->back_num   = 0;
    e->back_alloc = 0;
    e->back_refs  = (int* )NULL;

#else

    if (IS_NULL(t)) {
      alloc = INIT_NAMES_ALLOC_NUM;
      t = (NameTable* )xmalloc(sizeof(NameTable));
      CHECK_NULL_RETURN_MEMERR(t);
      t->e     = NULL;
      t->alloc = 0;
      t->num   = 0;

      t->e = (NameEntry* )xmalloc(sizeof(NameEntry) * alloc);
      if (IS_NULL(t->e)) {
        xfree(t);
        return ONIGERR_MEMORY;
      }
      t->alloc = alloc;
      reg->name_table = t;
      goto clear;
    }
    else if (t->num == t->alloc) {
      int i;

      alloc = t->alloc * 2;
      t->e = (NameEntry* )xrealloc(t->e, sizeof(NameEntry) * alloc);
      CHECK_NULL_RETURN_MEMERR(t->e);
      t->alloc = alloc;

    clear:
      for (i = t->num; i < t->alloc; i++) {
        t->e[i].name       = NULL;
        t->e[i].name_len   = 0;
        t->e[i].back_num   = 0;
        t->e[i].back_alloc = 0;
        t->e[i].back_refs  = (int* )NULL;
      }
    }
    e = &(t->e[t->num]);
    t->num++;
    e->name = onigenc_strdup(reg->enc, name, name_end);
    if (IS_NULL(e->name)) return ONIGERR_MEMORY;
    e->name_len = name_end - name;
#endif
  }

  if (e->back_num >= 1 &&
      ! IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_MULTIPLEX_DEFINITION_NAME)) {
    onig_scan_env_set_error_string(env, ONIGERR_MULTIPLEX_DEFINED_NAME,
                                   name, name_end);
    return ONIGERR_MULTIPLEX_DEFINED_NAME;
  }

  e->back_num++;
  if (e->back_num == 1) {
    e->back_ref1 = backref;
  }
  else {
    if (e->back_num == 2) {
      alloc = INIT_NAME_BACKREFS_ALLOC_NUM;
      e->back_refs = (int* )xmalloc(sizeof(int) * alloc);
      CHECK_NULL_RETURN_MEMERR(e->back_refs);
      e->back_alloc = alloc;
      e->back_refs[0] = e->back_ref1;
      e->back_refs[1] = backref;
    }
    else {
      if (e->back_num > e->back_alloc) {
        alloc = e->back_alloc * 2;
        e->back_refs = (int* )xrealloc(e->back_refs, sizeof(int) * alloc);
        CHECK_NULL_RETURN_MEMERR(e->back_refs);
        e->back_alloc = alloc;
      }
      e->back_refs[e->back_num - 1] = backref;
    }
  }

  return 0;
}