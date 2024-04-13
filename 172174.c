setup_ext_callout_list_values(regex_t* reg)
{
  int i, j;
  RegexExt* ext;

  ext = REG_EXTP(reg);
  if (IS_NOT_NULL(ext->tag_table)) {
    onig_st_foreach((CalloutTagTable *)ext->tag_table, i_callout_callout_list_set,
                    (st_data_t )ext);
  }

  for (i = 0; i < ext->callout_num; i++) {
    CalloutListEntry* e = ext->callout_list + i;
    if (e->of == ONIG_CALLOUT_OF_NAME) {
      for (j = 0; j < e->u.arg.num; j++) {
        if (e->u.arg.types[j] == ONIG_TYPE_TAG) {
          UChar* start;
          UChar* end;
          int num;
          start = e->u.arg.vals[j].s.start;
          end   = e->u.arg.vals[j].s.end;
          num = onig_get_callout_num_by_tag(reg, start, end);
          if (num < 0) return num;
          e->u.arg.vals[j].tag = num;
        }
      }
    }
  }

  return ONIG_NORMAL;
}