concat_opt_anc_info(OptAnc* to, OptAnc* left, OptAnc* right,
                    OnigLen left_len, OnigLen right_len)
{
  clear_opt_anc_info(to);

  to->left = left->left;
  if (left_len == 0) {
    to->left |= right->left;
  }

  to->right = right->right;
  if (right_len == 0) {
    to->right |= left->right;
  }
  else {
    to->right |= (left->right & ANCR_PREC_READ_NOT);
  }
}