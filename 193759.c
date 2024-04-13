generateReference(const char *custom1, const char *custom2)
{
    static uint ref_counter = 0;
    const char *local_cust1 = custom1;
    const char *local_cust2 = custom2;
    int reference_len = 4;
    char *since_epoch = NULL;

    reference_len += 20;        /* too big */
    reference_len += 40;        /* too big */

    if (local_cust1 == NULL) {
        local_cust1 = "_empty_";
    }
    reference_len += strlen(local_cust1);

    if (local_cust2 == NULL) {
        local_cust2 = "_empty_";
    }
    reference_len += strlen(local_cust2);

    since_epoch = calloc(1, reference_len);

    if (since_epoch != NULL) {
        sprintf(since_epoch, "%s-%s-%ld-%u",
                local_cust1, local_cust2, (unsigned long)time(NULL), ref_counter++);
    }

    return since_epoch;
}