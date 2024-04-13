unravel_case_fold_string(Node* node, regex_t* reg, int state)
{
  int r, n, in_look_behind;
  OnigLen min_len, max_len, one_len;
  UChar *start, *end, *p, *q;
  StrNode* snode;
  Node *sn, *list;
  OnigEncoding enc;
  OnigCaseFoldCodeItem items[ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM];

  if (NODE_STRING_IS_CASE_EXPANDED(node)) return 0;

  NODE_STATUS_REMOVE(node, IGNORECASE);
  snode = STR_(node);
  start = snode->s;
  end   = snode->end;
  if (start >= end) return 0;

  in_look_behind = (state & IN_LOOK_BEHIND) != 0;
  enc = reg->enc;

  list = sn = NULL_NODE;
  p = start;
  while (p < end) {
    n = ONIGENC_GET_CASE_FOLD_CODES_BY_STR(enc, reg->case_fold_flag, p, end,
                                           items);
    if (n < 0) {
      r = n;
      goto err;
    }

    one_len = (OnigLen )enclen(enc, p);
    if (n == 0) {
      q = p + one_len;
      if (q > end) q = end;
      r = unravel_cf_string_add(&list, &sn, p, q, 0 /* flag */);
      if (r != 0) goto err;
    }
    else {
      if (in_look_behind != 0) {
        q = p + one_len;
        if (items[0].byte_len != one_len) {
          r = ONIGENC_GET_CASE_FOLD_CODES_BY_STR(enc, reg->case_fold_flag, p, q,
                                                 items);
          if (r < 0) goto err;
          n = r;
        }
        r = unravel_cf_look_behind_add(&list, &sn, n, items, enc, p, one_len);
        if (r != 0) goto err;
      }
      else {
        get_min_max_byte_len_case_fold_items(n, items, &min_len, &max_len);
        if (min_len != max_len) {
          r = ONIGERR_PARSER_BUG;
          goto err;
        }

        q = p + max_len;
        r = unravel_cf_string_alt_or_cc_add(&list, n, items, enc,
                                            reg->case_fold_flag, p, q);
        if (r != 0) goto err;
        sn = NULL_NODE;
      }
    }

    p = q;
  }

  if (IS_NOT_NULL(list)) {
    if (node_list_len(list) == 1) {
      node_swap(node, NODE_CAR(list));
    }
    else {
      node_swap(node, list);
    }
    onig_node_free(list);
  }
  else {
    node_swap(node, sn);
    onig_node_free(sn);
  }
  return 0;

 err:
  if (IS_NOT_NULL(list))
    onig_node_free(list);
  else if (IS_NOT_NULL(sn))
    onig_node_free(sn);

  return r;
}