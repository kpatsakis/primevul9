alt_merge_opt_anc_info(OptAnc* to, OptAnc* add)
{
  to->left  &= add->left;
  to->right &= add->right;
}