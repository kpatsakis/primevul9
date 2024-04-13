lyp_check_mandatory_choice(struct lys_node *node)
{
    const struct lys_node *mand, *dflt = ((struct lys_node_choice *)node)->dflt;

    if ((mand = lyp_check_mandatory_(dflt))) {
        if (mand != dflt) {
            LOGVAL(node->module->ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "mandatory");
            LOGVAL(node->module->ctx, LYE_SPEC, LY_VLOG_NONE, NULL,
                   "Mandatory node \"%s\" is directly under the default case \"%s\" of the \"%s\" choice.",
                   mand->name, dflt->name, node->name);
            return -1;
        }
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}