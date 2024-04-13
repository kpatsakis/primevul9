finish_realms()
{
    int i;

    for (i = 0; i < shandle.kdc_numrealms; i++) {
        finish_realm(shandle.kdc_realmlist[i]);
        shandle.kdc_realmlist[i] = 0;
    }
    shandle.kdc_numrealms = 0;
}