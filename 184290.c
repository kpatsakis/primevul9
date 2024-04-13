make_code_list_to_string(Node** rnode, OnigEncoding enc,
                         int n, OnigCodePoint codes[])
{
  int r, i, len;
  Node* node;
  UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];

  *rnode = NULL_NODE;
  node = onig_node_new_str(NULL, NULL);
  CHECK_NULL_RETURN_MEMERR(node);

  for (i = 0; i < n; i++) {
    len = ONIGENC_CODE_TO_MBC(enc, codes[i], buf);
    if (len < 0) {
      r = len;
      goto err;
    }

    r = onig_node_str_cat(node, buf, buf + len);
    if (r != 0) goto err;
  }

  *rnode = node;
  return 0;

 err:
  onig_node_free(node);
  return r;
}