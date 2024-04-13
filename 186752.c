get_om_values(
    XOM om,
    XlcArgList args,
    int num_args)
{
    if (om->core.resources == NULL)
	return NULL;

    return _XlcGetValues((XPointer) om, om->core.resources,
			 om->core.num_resources, args, num_args, XlcGetMask);
}