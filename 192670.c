find(item* head, UV key)
{
    item* iterator = head;
    while (iterator){
        if (iterator->key == key){
            return iterator;
        }
        iterator = iterator->next;
    }

    return NULL;
}