unravel_cf_string_add(Node** rlist, Node** rsn, UChar* s, UChar* end,
                      unsigned int flag)
{
  int r;
  Node *sn, *list;

  list = *rlist;
  sn   = *rsn;

  if (IS_NOT_NULL(sn) && STR_(sn)->flag == flag) {
    r = onig_node_str_cat(sn, s, end);
  }
  else {
    sn = onig_node_new_str(s, end);
    CHECK_NULL_RETURN_MEMERR(sn);

    STR_(sn)->flag = flag;
    r = unravel_cf_node_add(&list, sn);
  }

  if (r == 0) {
    *rlist = list;
    *rsn = sn;
  }
  return r;
}