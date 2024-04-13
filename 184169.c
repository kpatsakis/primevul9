add_opt_anc_info(OptAnc* to, int anc)
{
  if (is_left(anc))
    to->left |= anc;
  else
    to->right |= anc;
}