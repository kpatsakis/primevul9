is_strict_real_node(Node* node)
{
  switch (NODE_TYPE(node)) {
  case NODE_STRING:
    {
      StrNode* sn = STR_(node);
      return (sn->end != sn->s);
    }
    break;

  case NODE_CCLASS:
  case NODE_CTYPE:
    return 1;
    break;

  default:
    return 0;
    break;
  }
}