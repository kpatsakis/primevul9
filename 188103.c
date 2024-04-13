quantify_property_node(Node **np, ScanEnv* env, const char* propname, char repetitions)
{
  int r;
  int lower = 0;
  int upper = REPEAT_INFINITE;

  r = create_property_node(np, env, propname);
  if (r != 0) return r;
  switch (repetitions) {
    case '?':  upper = 1;          break;
    case '+':  lower = 1;          break;
    case '*':                      break;
    case '2':  lower = upper = 2;  break;
    default :  return ONIGERR_PARSER_BUG;
  }
  return quantify_node(np, lower, upper);
}