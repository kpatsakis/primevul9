void BuildColorantList(char *Colorant, cmsUInt32Number nColorant, cmsUInt16Number Out[])
{
    char Buff[32];
    cmsUInt32Number j;

    Colorant[0] = 0;
    if (nColorant > cmsMAXCHANNELS)
        nColorant = cmsMAXCHANNELS;

    for (j = 0; j < nColorant; j++) {

        snprintf(Buff, 31, "%.3f", Out[j] / 65535.0);
        Buff[31] = 0;
        strcat(Colorant, Buff);
        if (j < nColorant - 1)
            strcat(Colorant, " ");

    }
}