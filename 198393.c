user_func_error(int error, char_u *name, funcexe_T *funcexe)
{
    switch (error)
    {
	case FCERR_UNKNOWN:
		if (funcexe->fe_found_var)
		    semsg(_(e_not_callable_type_str), name);
		else
		    emsg_funcname(e_unknown_function_str, name);
		break;
	case FCERR_NOTMETHOD:
		emsg_funcname(
			N_("E276: Cannot use function as a method: %s"), name);
		break;
	case FCERR_DELETED:
		emsg_funcname(e_func_deleted, name);
		break;
	case FCERR_TOOMANY:
		emsg_funcname((char *)e_too_many_arguments_for_function_str,
									 name);
		break;
	case FCERR_TOOFEW:
		emsg_funcname((char *)e_not_enough_arguments_for_function_str,
									 name);
		break;
	case FCERR_SCRIPT:
		emsg_funcname(
		    e_using_sid_not_in_script_context_str, name);
		break;
	case FCERR_DICT:
		emsg_funcname(
		      N_("E725: Calling dict function without Dictionary: %s"),
			name);
		break;
    }
}