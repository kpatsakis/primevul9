crm_client_name(crm_client_t * c)
{
    if (c == NULL) {
        return "null";
    } else if (c->name == NULL && c->id == NULL) {
        return "unknown";
    } else if (c->name == NULL) {
        return c->id;
    } else {
        return c->name;
    }
}