static void button_lid_switch_handle_action(Manager *manager, bool is_edge) {
        HandleAction handle_action;

        assert(manager);

        /* If we are docked or on external power, handle the lid switch
         * differently */
        if (manager_is_docked_or_external_displays(manager))
                handle_action = manager->handle_lid_switch_docked;
        else if (manager->handle_lid_switch_ep != _HANDLE_ACTION_INVALID &&
                 manager_is_on_external_power())
                handle_action = manager->handle_lid_switch_ep;
        else
                handle_action = manager->handle_lid_switch;

        manager_handle_action(manager, INHIBIT_HANDLE_LID_SWITCH, handle_action, manager->lid_switch_ignore_inhibited, is_edge);
}