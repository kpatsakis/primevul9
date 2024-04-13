get_oc_values(
    XOC oc,
    XlcArgList args,
    int num_args)
{
    if (oc->core.resources == NULL)
	return NULL;

    return _XlcGetValues((XPointer) oc, oc->core.resources,
			 oc->core.num_resources, args, num_args, XlcGetMask);
}