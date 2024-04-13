subOpCallback(void * /*subOpCallbackData*/ ,
        T_ASC_Network *aNet, T_ASC_Association **subAssoc)
{

    if (aNet == NULL) return;   /* help no net ! */

    if (*subAssoc == NULL) {
        /* negotiate association */
        acceptSubAssoc(aNet, subAssoc);
    } else {
        /* be a service class provider */
        subOpSCP(subAssoc);
    }
}