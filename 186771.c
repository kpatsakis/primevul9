add_data(
    XOM om)
{
    XOMGenericPart *gen = XOM_GENERIC(om);
    OMData new;
    int num;

    if ((num = gen->data_num))
        new = Xrealloc(gen->data, (num + 1) * sizeof(OMDataRec));
    else
        new = Xmalloc(sizeof(OMDataRec));

    if (new == NULL)
        return NULL;

    gen->data_num = num + 1;
    gen->data = new;

    new += num;
    bzero((char *) new, sizeof(OMDataRec));

    return new;
}