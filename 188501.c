GuestLogicalProcessorList *qmp_guest_get_vcpus(Error **errp)
{
    int64_t current;
    GuestLogicalProcessorList *head, **link;
    long sc_max;
    Error *local_err = NULL;

    current = 0;
    head = NULL;
    link = &head;
    sc_max = SYSCONF_EXACT(_SC_NPROCESSORS_CONF, &local_err);

    while (local_err == NULL && current < sc_max) {
        GuestLogicalProcessor *vcpu;
        GuestLogicalProcessorList *entry;

        vcpu = g_malloc0(sizeof *vcpu);
        vcpu->logical_id = current++;
        vcpu->has_can_offline = true; /* lolspeak ftw */
        transfer_vcpu(vcpu, true, &local_err);

        entry = g_malloc0(sizeof *entry);
        entry->value = vcpu;

        *link = entry;
        link = &entry->next;
    }

    if (local_err == NULL) {
        /* there's no guest with zero VCPUs */
        g_assert(head != NULL);
        return head;
    }

    qapi_free_GuestLogicalProcessorList(head);
    error_propagate(errp, local_err);
    return NULL;
}