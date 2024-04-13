Perl_current_re_engine(pTHX)
{
    if (IN_PERL_COMPILETIME) {
	HV * const table = GvHV(PL_hintgv);
	SV **ptr;

	if (!table || !(PL_hints & HINT_LOCALIZE_HH))
	    return &PL_core_reg_engine;
	ptr = hv_fetchs(table, "regcomp", FALSE);
	if ( !(ptr && SvIOK(*ptr) && SvIV(*ptr)))
	    return &PL_core_reg_engine;
	return INT2PTR(regexp_engine*,SvIV(*ptr));
    }
    else {
	SV *ptr;
	if (!PL_curcop->cop_hints_hash)
	    return &PL_core_reg_engine;
	ptr = cop_hints_fetch_pvs(PL_curcop, "regcomp", 0);
	if ( !(ptr && SvIOK(ptr) && SvIV(ptr)))
	    return &PL_core_reg_engine;
	return INT2PTR(regexp_engine*,SvIV(ptr));
    }
}