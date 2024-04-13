onig_node_str_cat(Node* node, const UChar* s, const UChar* end)
{
  int addlen = end - s;

  if (addlen > 0) {
    int len  = STR_(node)->end - STR_(node)->s;

    if (STR_(node)->capa > 0 || (len + addlen > NODE_STR_BUF_SIZE - 1)) {
      UChar* p;
      int capa = len + addlen + NODE_STR_MARGIN;

      if (capa <= STR_(node)->capa) {
        onig_strcpy(STR_(node)->s + len, s, end);
      }
      else {
        if (STR_(node)->s == STR_(node)->buf)
          p = strcat_capa_from_static(STR_(node)->s, STR_(node)->end,
                                      s, end, capa);
        else
          p = strcat_capa(STR_(node)->s, STR_(node)->end, s, end, capa);

        CHECK_NULL_RETURN_MEMERR(p);
        STR_(node)->s    = p;
        STR_(node)->capa = capa;
      }
    }
    else {
      onig_strcpy(STR_(node)->s + len, s, end);
    }
    STR_(node)->end = STR_(node)->s + len + addlen;
  }

  return 0;
}