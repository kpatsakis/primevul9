renumber_backref_node(Node* node, GroupNumMap* map)
{
  int i, pos, n, old_num;
  int *backs;
  BackRefNode* bn = BACKREF_(node);

  if (! NODE_IS_BY_NAME(node))
    return ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED;

  old_num = bn->back_num;
  if (IS_NULL(bn->back_dynamic))
    backs = bn->back_static;
  else
    backs = bn->back_dynamic;

  for (i = 0, pos = 0; i < old_num; i++) {
    n = map[backs[i]].new_val;
    if (n > 0) {
      backs[pos] = n;
      pos++;
    }
  }

  bn->back_num = pos;
  return 0;
}