guest_find_interface(GuestNetworkInterfaceList *head,
                     const char *name)
{
    for (; head; head = head->next) {
        if (strcmp(head->value->name, name) == 0) {
            break;
        }
    }

    return head;
}