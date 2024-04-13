make_named_capture_number_map(Node** plink, GroupNumMap* map, int* counter)
{
  int r = 0;
  Node* node = *plink;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = make_named_capture_number_map(&(NODE_CAR(node)), map, counter);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    {
      Node** ptarget = &(NODE_BODY(node));
      Node*  old = *ptarget;
      r = make_named_capture_number_map(ptarget, map, counter);
      if (r != 0) return r;
      if (*ptarget != old && NODE_TYPE(*ptarget) == NODE_QUANT) {
        r = onig_reduce_nested_quantifier(node);
      }
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);
      if (en->type == BAG_MEMORY) {
        if (NODE_IS_NAMED_GROUP(node)) {
          (*counter)++;
          map[en->m.regnum].new_val = *counter;
          en->m.regnum = *counter;
          r = make_named_capture_number_map(&(NODE_BODY(node)), map, counter);
        }
        else {
          *plink = NODE_BODY(node);
          NODE_BODY(node) = NULL_NODE;
          onig_node_free(node);
          r = make_named_capture_number_map(plink, map, counter);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        r = make_named_capture_number_map(&(NODE_BAG_BODY(en)), map, counter);
        if (r != 0) return r;
        if (IS_NOT_NULL(en->te.Then)) {
          r = make_named_capture_number_map(&(en->te.Then), map, counter);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = make_named_capture_number_map(&(en->te.Else), map, counter);
          if (r != 0) return r;
        }
      }
      else
        r = make_named_capture_number_map(&(NODE_BODY(node)), map, counter);
    }
    break;

  case NODE_ANCHOR:
    if (IS_NOT_NULL(NODE_BODY(node)))
      r = make_named_capture_number_map(&(NODE_BODY(node)), map, counter);
    break;

  default:
    break;
  }

  return r;
}