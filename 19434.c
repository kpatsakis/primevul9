del_mouse_termcode(
    int		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
#   ifdef FEAT_MOUSE_JSB
    if (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    else
#   endif
#   ifdef FEAT_MOUSE_NET
    if (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    else
#   endif
#   ifdef FEAT_MOUSE_DEC
    if (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    else
#   endif
#   ifdef FEAT_MOUSE_PTERM
    if (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    else
#   endif
#   ifdef FEAT_MOUSE_URXVT
    if (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    else
#   endif
#   ifdef FEAT_MOUSE_GPM
    if (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    else
#   endif
    if (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    else if (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    else
	has_mouse_termcode &= ~HMT_NORMAL;
}