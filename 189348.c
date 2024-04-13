get_varp_scope(struct vimoption *p, int opt_flags)
{
    if ((opt_flags & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	if (p->var == VAR_WIN)
	    return (char_u *)GLOBAL_WO(get_varp(p));
	return p->var;
    }
    if ((opt_flags & OPT_LOCAL) && ((int)p->indir & PV_BOTH))
    {
	switch ((int)p->indir)
	{
#ifdef FEAT_QUICKFIX
	    case PV_EFM:  return (char_u *)&(curbuf->b_p_efm);
	    case PV_GP:   return (char_u *)&(curbuf->b_p_gp);
	    case PV_MP:   return (char_u *)&(curbuf->b_p_mp);
#endif
	    case PV_EP:   return (char_u *)&(curbuf->b_p_ep);
	    case PV_KP:   return (char_u *)&(curbuf->b_p_kp);
	    case PV_PATH: return (char_u *)&(curbuf->b_p_path);
	    case PV_AR:   return (char_u *)&(curbuf->b_p_ar);
	    case PV_TAGS: return (char_u *)&(curbuf->b_p_tags);
	    case PV_TC:   return (char_u *)&(curbuf->b_p_tc);
#ifdef FEAT_FIND_ID
	    case PV_DEF:  return (char_u *)&(curbuf->b_p_def);
	    case PV_INC:  return (char_u *)&(curbuf->b_p_inc);
#endif
#ifdef FEAT_INS_EXPAND
	    case PV_DICT: return (char_u *)&(curbuf->b_p_dict);
	    case PV_TSR:  return (char_u *)&(curbuf->b_p_tsr);
#endif
#if defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    case PV_BEXPR: return (char_u *)&(curbuf->b_p_bexpr);
#endif
#if defined(FEAT_CRYPT)
	    case PV_CM:	  return (char_u *)&(curbuf->b_p_cm);
#endif
#ifdef FEAT_STL_OPT
	    case PV_STL:  return (char_u *)&(curwin->w_p_stl);
#endif
	    case PV_UL:   return (char_u *)&(curbuf->b_p_ul);
#ifdef FEAT_LISP
	    case PV_LW:   return (char_u *)&(curbuf->b_p_lw);
#endif
	    case PV_BKC:  return (char_u *)&(curbuf->b_p_bkc);
	}
	return NULL; /* "cannot happen" */
    }
    return get_varp(p);
}