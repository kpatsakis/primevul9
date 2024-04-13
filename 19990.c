void memory_listener_register(MemoryListener *listener, AddressSpace *as)
{
    listener->address_space = as;
    QTAILQ_INSERT_TAIL(&as->uc->memory_listeners, listener, link);
    QTAILQ_INSERT_TAIL(&as->listeners, listener, link_as);

    listener_add_address_space(listener, as);
}