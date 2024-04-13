is_set_opt_anc_info(OptAnc* to, int anc)
{
  if ((to->left & anc) != 0) return 1;

  return ((to->right & anc) != 0 ? 1 : 0);
}