node_swap(Node* a, Node* b)
{
  Node c;

  c = *a; *a = *b; *b = c;

  if (NODE_TYPE(a) == NODE_STRING) {
    StrNode* sn = STR_(a);
    if (sn->capacity == 0) {
      int len = (int )(sn->end - sn->s);
      sn->s   = sn->buf;
      sn->end = sn->s + len;
    }
  }

  if (NODE_TYPE(b) == NODE_STRING) {
    StrNode* sn = STR_(b);
    if (sn->capacity == 0) {
      int len = (int )(sn->end - sn->s);
      sn->s   = sn->buf;
      sn->end = sn->s + len;
    }
  }
}