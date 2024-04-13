get_expr_name(expr_ty e)
{
    switch (e->kind) {
        case Attribute_kind:
            return "attribute";
        case Subscript_kind:
            return "subscript";
        case Starred_kind:
            return "starred";
        case Name_kind:
            return "name";
        case List_kind:
            return "list";
        case Tuple_kind:
            return "tuple";
        case Lambda_kind:
            return "lambda";
        case Call_kind:
            return "function call";
        case BoolOp_kind:
        case BinOp_kind:
        case UnaryOp_kind:
            return "operator";
        case GeneratorExp_kind:
            return "generator expression";
        case Yield_kind:
        case YieldFrom_kind:
            return "yield expression";
        case Await_kind:
            return "await expression";
        case ListComp_kind:
            return "list comprehension";
        case SetComp_kind:
            return "set comprehension";
        case DictComp_kind:
            return "dict comprehension";
        case Dict_kind:
            return "dict display";
        case Set_kind:
            return "set display";
        case JoinedStr_kind:
        case FormattedValue_kind:
            return "f-string expression";
        case Constant_kind: {
            PyObject *value = e->v.Constant.value;
            if (value == Py_None) {
                return "None";
            }
            if (value == Py_False) {
                return "False";
            }
            if (value == Py_True) {
                return "True";
            }
            if (value == Py_Ellipsis) {
                return "Ellipsis";
            }
            return "literal";
        }
        case Compare_kind:
            return "comparison";
        case IfExp_kind:
            return "conditional expression";
        case NamedExpr_kind:
            return "named expression";
        default:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            return NULL;
    }
}