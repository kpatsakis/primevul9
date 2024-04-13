static void   _clean_var(VAR *var)
{
    if (var->name) {
      free(var->name);
    }
    if (var->defval && (&quote != var->defval)) {
      free(var->defval);
    }
    if (var->override && (&quote != var->override)) {
      free(var->override);
    }
    var->name = NULL;
    var->value = NULL;    /* never has memory specific to it */
    var->defval = NULL;
    var->override = NULL;
    return;
}