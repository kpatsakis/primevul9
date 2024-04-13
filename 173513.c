is_invalid_quantifier_target(Node* node)
{
  switch (NODE_TYPE(node)) {
  case NODE_ANCHOR:
    return 1;
    break;

  case NODE_ENCLOSURE:
    /* allow enclosed elements */
    /* return is_invalid_quantifier_target(NODE_BODY(node)); */
    break;

  case NODE_LIST:
    do {
      if (! is_invalid_quantifier_target(NODE_CAR(node))) return 0;
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    return 0;
    break;

  case NODE_ALT:
    do {
      if (is_invalid_quantifier_target(NODE_CAR(node))) return 1;
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  default:
    break;
  }
  return 0;
}