expand_case_fold_string_alt(int item_num, OnigCaseFoldCodeItem items[], UChar *p,
                            int slen, UChar *end, regex_t* reg, Node **rnode)
{
  int r, i, j;
  int len;
  int varlen;
  Node *anode, *var_anode, *snode, *xnode, *an;
  UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];

  *rnode = var_anode = NULL_NODE;

  varlen = 0;
  for (i = 0; i < item_num; i++) {
    if (items[i].byte_len != slen) {
      varlen = 1;
      break;
    }
  }

  if (varlen != 0) {
    *rnode = var_anode = onig_node_new_alt(NULL_NODE, NULL_NODE);
    if (IS_NULL(var_anode)) return ONIGERR_MEMORY;

    xnode = onig_node_new_list(NULL, NULL);
    if (IS_NULL(xnode)) goto mem_err;
    NODE_CAR(var_anode) = xnode;

    anode = onig_node_new_alt(NULL_NODE, NULL_NODE);
    if (IS_NULL(anode)) goto mem_err;
    NODE_CAR(xnode) = anode;
  }
  else {
    *rnode = anode = onig_node_new_alt(NULL_NODE, NULL_NODE);
    if (IS_NULL(anode)) return ONIGERR_MEMORY;
  }

  snode = onig_node_new_str(p, p + slen);
  if (IS_NULL(snode)) goto mem_err;

  NODE_CAR(anode) = snode;

  for (i = 0; i < item_num; i++) {
    snode = onig_node_new_str(NULL, NULL);
    if (IS_NULL(snode)) goto mem_err;
    
    for (j = 0; j < items[i].code_len; j++) {
      len = ONIGENC_CODE_TO_MBC(reg->enc, items[i].code[j], buf);
      if (len < 0) {
        r = len;
        goto mem_err2;
      }

      r = onig_node_str_cat(snode, buf, buf + len);
      if (r != 0) goto mem_err2;
    }

    an = onig_node_new_alt(NULL_NODE, NULL_NODE);
    if (IS_NULL(an)) {
      goto mem_err2;
    }

    if (items[i].byte_len != slen) {
      Node *rem;
      UChar *q = p + items[i].byte_len;

      if (q < end) {
        r = expand_case_fold_make_rem_string(&rem, q, end, reg);
        if (r != 0) {
          onig_node_free(an);
          goto mem_err2;
        }

        xnode = onig_node_list_add(NULL_NODE, snode);
        if (IS_NULL(xnode)) {
          onig_node_free(an);
          onig_node_free(rem);
          goto mem_err2;
        }
        if (IS_NULL(onig_node_list_add(xnode, rem))) {
          onig_node_free(an);
          onig_node_free(xnode);
          onig_node_free(rem);
          goto mem_err;
        }

        NODE_CAR(an) = xnode;
      }
      else {
        NODE_CAR(an) = snode;
      }

      NODE_CDR(var_anode) = an;
      var_anode = an;
    }
    else {
      NODE_CAR(an)     = snode;
      NODE_CDR(anode) = an;
      anode = an;
    }
  }

  return varlen;

 mem_err2:
  onig_node_free(snode);

 mem_err:
  onig_node_free(*rnode);

  return ONIGERR_MEMORY;
}