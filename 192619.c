dict_free(item* head)
{
    item* iterator = head;

    while (iterator) {
        item* temp = iterator;
        iterator = iterator->next;
        Safefree(temp);
    }

    head = NULL;
}