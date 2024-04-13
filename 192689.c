uniquePush(item* head,UV key)
{
    item* iterator = head;

    while (iterator){
        if (iterator->key == key) {
            return head;
        }
        iterator = iterator->next;
    }

    return push(key,head);
}