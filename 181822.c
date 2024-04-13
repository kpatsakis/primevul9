lyp_check_mandatory_augment(struct lys_node_augment *aug, const struct lys_node *target)
{
    const struct lys_node *node;

    if (aug->when || target->nodetype == LYS_CHOICE) {
        /* - mandatory nodes in new cases are ok;
         * clarification from YANG 1.1 - augmentation can add mandatory nodes when it is
         * conditional with a when statement */
        return EXIT_SUCCESS;
    }

    if ((node = lyp_check_mandatory_((struct lys_node *)aug))) {
        if (node != (struct lys_node *)aug) {
            LOGVAL(target->module->ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "mandatory");
            LOGVAL(target->module->ctx, LYE_SPEC, LY_VLOG_NONE, NULL,
                   "Mandatory node \"%s\" appears in augment of \"%s\" without when condition.",
                   node->name, aug->target_name);
            return -1;
        }
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}