set_sub_anchor(regex_t* reg, OptAnc* anc)
{
  reg->sub_anchor |= anc->left  & ANCR_BEGIN_LINE;
  reg->sub_anchor |= anc->right & ANCR_END_LINE;
}