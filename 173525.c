node_new_call(UChar* name, UChar* name_end, int gnum)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NODE_TYPE(node, NODE_CALL);
  CALL_(node)->name      = name;
  CALL_(node)->name_end  = name_end;
  CALL_(node)->group_num = gnum;  /* call by number if gnum != 0 */
  return node;
}