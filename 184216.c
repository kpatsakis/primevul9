unravel_cf_string_alt_or_cc_add(Node** rlist, int n,
            OnigCaseFoldCodeItem items[], OnigEncoding enc,
            OnigCaseFoldType case_fold_flag, UChar* s, UChar* end)
{
  int r, i;
  Node* node;

  if (is_all_code_len_1_items(n, items)) {
    OnigCodePoint codes[14];/* least ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM + 1 */

    codes[0] = ONIGENC_MBC_TO_CODE(enc, s, end);
    for (i = 0; i < n; i++) {
      OnigCaseFoldCodeItem* item = items + i;
      codes[i+1] = item->code[0];
    }
    r = onig_new_cclass_with_code_list(&node, enc, n + 1, codes);
    if (r != 0) return r;
  }
  else {
    Node *snode, *alt, *curr;

    snode = onig_node_new_str(s, end);
    CHECK_NULL_RETURN_MEMERR(snode);
    node = curr = onig_node_new_alt(snode, NULL_NODE);
    if (IS_NULL(curr)) {
      onig_node_free(snode);
      return ONIGERR_MEMORY;
    }

    r = 0;
    for (i = 0; i < n; i++) {
      OnigCaseFoldCodeItem* item = items + i;
      r = make_code_list_to_string(&snode, enc, item->code_len, item->code);
      if (r != 0) {
        onig_node_free(node);
        return r;
      }

      alt = onig_node_new_alt(snode, NULL_NODE);
      if (IS_NULL(alt)) {
        onig_node_free(snode);
        onig_node_free(node);
        return ONIGERR_MEMORY;
      }

      NODE_CDR(curr) = alt;
      curr = alt;
    }
  }

  r = unravel_cf_node_add(rlist, node);
  if (r != 0) onig_node_free(node);
  return r;
}