callout_tag_entry(regex_t* reg, UChar* name, UChar* name_end,
                  CalloutTagVal entry_val)
{
  int r;
  RegexExt* ext;
  CalloutListEntry* e;

  r = ext_ensure_tag_table(reg);
  if (r != ONIG_NORMAL) return r;

  ext = onig_get_regex_ext(reg);
  r = callout_tag_entry_raw(ext->tag_table, name, name_end, entry_val);

  e = onig_reg_callout_list_at(reg, (int )entry_val);
  e->tag_start = name;
  e->tag_end   = name_end;

  return r;
}