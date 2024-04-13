create_property_node(Node **np, ScanEnv* env, const char* propname)
{
  int r;
  CClassNode* cc;

  *np = node_new_cclass();
  if (IS_NULL(*np)) return ONIGERR_MEMORY;
  cc = NCCLASS(*np);
  r = add_property_to_cc(cc, propname, 0, env);
  if (r != 0)
    onig_node_free(*np);
  return r;
}