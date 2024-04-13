push(UV key,item* curr)
{
    item* head;
    Newxz(head, 1, item);
    head->key = key;
    head->value = 0;
    head->next = curr;
    return head;
}