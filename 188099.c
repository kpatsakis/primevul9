add_property_to_cc(CClassNode* cc, const char* propname, int not, ScanEnv* env)
{
  int ctype = propname2ctype(env, propname);
  if (ctype < 0) return ctype;
  return add_ctype_to_cc(cc, ctype, not, 0, env);
}