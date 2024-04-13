get_char_length_tree1(Node* node, regex_t* reg, int* len, int level)
{
  int tlen;
  int r = 0;

  level++;
  *len = 0;
  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    do {
      r = get_char_length_tree1(NODE_CAR(node), reg, &tlen, level);
      if (r == 0)
        *len = distance_add(*len, tlen);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ALT:
    {
      int tlen2;
      int varlen = 0;

      r = get_char_length_tree1(NODE_CAR(node), reg, &tlen, level);
      while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node))) {
        r = get_char_length_tree1(NODE_CAR(node), reg, &tlen2, level);
        if (r == 0) {
          if (tlen != tlen2)
            varlen = 1;
        }
      }
      if (r == 0) {
        if (varlen != 0) {
          if (level == 1)
            r = GET_CHAR_LEN_TOP_ALT_VARLEN;
          else
            r = GET_CHAR_LEN_VARLEN;
        }
        else
          *len = tlen;
      }
    }
    break;

  case NODE_STRING:
    {
      StrNode* sn = STR_(node);
      UChar *s = sn->s;

      while (s < sn->end) {
        s += enclen(reg->enc, s);
        (*len)++;
      }
    }
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->lower == qn->upper) {
        if (qn->upper == 0) {
          *len = 0;
        }
        else {
          r = get_char_length_tree1(NODE_BODY(node), reg, &tlen, level);
          if (r == 0)
            *len = distance_multiply(tlen, qn->lower);
        }
      }
      else
        r = GET_CHAR_LEN_VARLEN;
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    if (! NODE_IS_RECURSION(node))
      r = get_char_length_tree1(NODE_BODY(node), reg, len, level);
    else
      r = GET_CHAR_LEN_VARLEN;
    break;
#endif

  case NODE_CTYPE:
  case NODE_CCLASS:
    *len = 1;
    break;

  case NODE_ENCLOSURE:
    {
      EnclosureNode* en = ENCLOSURE_(node);

      switch (en->type) {
      case ENCLOSURE_MEMORY:
#ifdef USE_CALL
        if (NODE_IS_CLEN_FIXED(node))
          *len = en->char_len;
        else {
          r = get_char_length_tree1(NODE_BODY(node), reg, len, level);
          if (r == 0) {
            en->char_len = *len;
            NODE_STATUS_ADD(node, NST_CLEN_FIXED);
          }
        }
        break;
#endif
      case ENCLOSURE_OPTION:
      case ENCLOSURE_STOP_BACKTRACK:
        r = get_char_length_tree1(NODE_BODY(node), reg, len, level);
        break;
      case ENCLOSURE_IF_ELSE:
        {
          int clen, elen;

          r = get_char_length_tree1(NODE_BODY(node), reg, &clen, level);
          if (r == 0) {
            if (IS_NOT_NULL(en->te.Then)) {
              r = get_char_length_tree1(en->te.Then, reg, &tlen, level);
              if (r != 0) break;
            }
            else tlen = 0;
            if (IS_NOT_NULL(en->te.Else)) {
              r = get_char_length_tree1(en->te.Else, reg, &elen, level);
              if (r != 0) break;
            }
            else elen = 0;

            if (clen + tlen != elen) {
              r = GET_CHAR_LEN_VARLEN;
            }
            else {
              *len = elen;
            }
          }
        }
        break;

      default:
        break;
      }
    }
    break;

  case NODE_ANCHOR:
  case NODE_GIMMICK:
    break;

  case NODE_BACKREF:
    if (NODE_IS_CHECKER(node))
      break;
    /* fall */
  default:
    r = GET_CHAR_LEN_VARLEN;
    break;
  }

  return r;
}