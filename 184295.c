reduce_string_list(Node* node, OnigEncoding enc)
{
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    {
      Node* prev;
      Node* curr;
      Node* prev_node;
      Node* next_node;

      prev = NULL_NODE;
      do {
        next_node = NODE_CDR(node);
        curr = NODE_CAR(node);
        if (NODE_TYPE(curr) == NODE_STRING) {
          if (IS_NULL(prev)
              || STR_(curr)->flag  != STR_(prev)->flag
              || NODE_STATUS(curr) != NODE_STATUS(prev)) {
            prev = curr;
            prev_node = node;
          }
          else {
            r = node_str_node_cat(prev, curr);
            if (r != 0) return r;
            remove_from_list(prev_node, node);
            onig_node_free(node);
          }
        }
        else {
          if (IS_NOT_NULL(prev)) {
#ifdef USE_CHECK_VALIDITY_OF_STRING_IN_TREE
            StrNode* sn = STR_(prev);
            if (! ONIGENC_IS_VALID_MBC_STRING(enc, sn->s, sn->end))
              return ONIGERR_INVALID_WIDE_CHAR_VALUE;
#endif
            prev = NULL_NODE;
          }
          r = reduce_string_list(curr, enc);
          if (r != 0) return r;
          prev_node = node;
        }

        node = next_node;
      } while (r == 0 && IS_NOT_NULL(node));

#ifdef USE_CHECK_VALIDITY_OF_STRING_IN_TREE
      if (IS_NOT_NULL(prev)) {
        StrNode* sn = STR_(prev);
        if (! ONIGENC_IS_VALID_MBC_STRING(enc, sn->s, sn->end))
          return ONIGERR_INVALID_WIDE_CHAR_VALUE;
      }
#endif
    }
    break;

  case NODE_ALT:
    do {
      r = reduce_string_list(NODE_CAR(node), enc);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

#ifdef USE_CHECK_VALIDITY_OF_STRING_IN_TREE
  case NODE_STRING:
    {
      StrNode* sn = STR_(node);
      if (! ONIGENC_IS_VALID_MBC_STRING(enc, sn->s, sn->end))
        return ONIGERR_INVALID_WIDE_CHAR_VALUE;
    }
    break;
#endif

  case NODE_ANCHOR:
    if (IS_NULL(NODE_BODY(node)))
      break;
    /* fall */
  case NODE_QUANT:
    r = reduce_string_list(NODE_BODY(node), enc);
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      r = reduce_string_list(NODE_BODY(node), enc);
      if (r != 0) return r;

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = reduce_string_list(en->te.Then, enc);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = reduce_string_list(en->te.Else, enc);
          if (r != 0) return r;
        }
      }
    }
    break;

  default:
    break;
  }

  return r;
}