unravel_cf_look_behind_add(Node** rlist, Node** rsn,
                int n, OnigCaseFoldCodeItem items[], OnigEncoding enc,
                UChar* s, OnigLen one_len)
{
  int r, i, found;

  found = FALSE;
  for (i = 0; i < n; i++) {
    OnigCaseFoldCodeItem* item = items + i;
    if (item->byte_len == one_len) {
      if (item->code_len == 1) {
        found = TRUE;
        break;
      }
    }
  }

  if (found == FALSE) {
    r = unravel_cf_string_add(rlist, rsn, s, s + one_len, 0 /* flag */);
  }
  else {
    Node* node;
    OnigCodePoint codes[14];/* least ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM + 1 */

    found = 0;
    codes[found++] = ONIGENC_MBC_TO_CODE(enc, s, s + one_len);
    for (i = 0; i < n; i++) {
      OnigCaseFoldCodeItem* item = items + i;
      if (item->byte_len == one_len) {
        if (item->code_len == 1) {
          codes[found++] = item->code[0];
        }
      }
    }
    r = onig_new_cclass_with_code_list(&node, enc, found, codes);
    if (r != 0) return r;

    r = unravel_cf_node_add(rlist, node);
    if (r != 0) onig_node_free(node);

    *rsn = NULL_NODE;
  }

  return r;
}