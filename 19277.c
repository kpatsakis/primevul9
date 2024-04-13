
static void
xmlXPathOptimizeExpression(xmlXPathCompExprPtr comp, xmlXPathStepOpPtr op)
{
    /*
    * Try to rewrite "descendant-or-self::node()/foo" to an optimized
    * internal representation.
    */

    if ((op->op == XPATH_OP_COLLECT /* 11 */) &&
        (op->ch1 != -1) &&
        (op->ch2 == -1 /* no predicate */))
    {
        xmlXPathStepOpPtr prevop = &comp->steps[op->ch1];

        if ((prevop->op == XPATH_OP_COLLECT /* 11 */) &&
            ((xmlXPathAxisVal) prevop->value ==
                AXIS_DESCENDANT_OR_SELF) &&
            (prevop->ch2 == -1) &&
            ((xmlXPathTestVal) prevop->value2 == NODE_TEST_TYPE) &&
            ((xmlXPathTypeVal) prevop->value3 == NODE_TYPE_NODE))
        {
            /*
            * This is a "descendant-or-self::node()" without predicates.
            * Try to eliminate it.
            */

            switch ((xmlXPathAxisVal) op->value) {
                case AXIS_CHILD:
                case AXIS_DESCENDANT:
                    /*
                    * Convert "descendant-or-self::node()/child::" or
                    * "descendant-or-self::node()/descendant::" to
                    * "descendant::"
                    */
                    op->ch1   = prevop->ch1;
                    op->value = AXIS_DESCENDANT;
                    break;
                case AXIS_SELF:
                case AXIS_DESCENDANT_OR_SELF:
                    /*
                    * Convert "descendant-or-self::node()/self::" or
                    * "descendant-or-self::node()/descendant-or-self::" to
                    * to "descendant-or-self::"
                    */
                    op->ch1   = prevop->ch1;
                    op->value = AXIS_DESCENDANT_OR_SELF;
                    break;
                default:
                    break;
            }
	}
    }

    /* Recurse */
    if (op->ch1 != -1)
        xmlXPathOptimizeExpression(comp, &comp->steps[op->ch1]);
    if (op->ch2 != -1)