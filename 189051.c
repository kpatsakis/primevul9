static int _check_var(pam_handle_t *pamh, VAR *var)
{
  /*
   * Examine the variable and determine what action to take.
   * Returns DEFINE_VAR, UNDEFINE_VAR depending on action to take
   * or a PAM_* error code if passed back from other routines
   *
   * if no DEFAULT provided, the empty string is assumed
   * if no OVERRIDE provided, the empty string is assumed
   * if DEFAULT=  and OVERRIDE evaluates to the empty string,
   *    this variable should be undefined
   * if DEFAULT=""  and OVERRIDE evaluates to the empty string,
   *    this variable should be defined with no value
   * if OVERRIDE=value   and value turns into the empty string, DEFAULT is used
   *
   * If DEFINE_VAR is to be returned, the correct value to define will
   * be pointed to by var->value
   */

  int retval;

  D(("Called."));

  /*
   * First thing to do is to expand any arguments, but only
   * if they are not the special quote values (cause expand_arg
   * changes memory).
   */

  if (var->defval && (&quote != var->defval) &&
      ((retval = _expand_arg(pamh, &(var->defval))) != PAM_SUCCESS)) {
      return retval;
  }
  if (var->override && (&quote != var->override) &&
      ((retval = _expand_arg(pamh, &(var->override))) != PAM_SUCCESS)) {
    return retval;
  }

  /* Now its easy */

  if (var->override && *(var->override) && &quote != var->override) {
    /* if there is a non-empty string in var->override, we use it */
    D(("OVERRIDE variable <%s> being used: <%s>", var->name, var->override));
    var->value = var->override;
    retval = DEFINE_VAR;
  } else {

    var->value = var->defval;
    if (&quote == var->defval) {
      /*
       * This means that the empty string was given for defval value
       * which indicates that a variable should be defined with no value
       */
      *var->defval = '\0';
      D(("An empty variable: <%s>", var->name));
      retval = DEFINE_VAR;
    } else if (var->defval) {
      D(("DEFAULT variable <%s> being used: <%s>", var->name, var->defval));
      retval = DEFINE_VAR;
    } else {
      D(("UNDEFINE variable <%s>", var->name));
      retval = UNDEFINE_VAR;
    }
  }

  D(("Exit."));
  return retval;
}