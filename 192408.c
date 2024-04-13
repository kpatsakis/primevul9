static VALUE cJSON_parser_s_allocate(VALUE klass)
{
    JSON_Parser *json;
    VALUE obj = TypedData_Make_Struct(klass, JSON_Parser, &JSON_Parser_type, json);
    json->fbuffer = fbuffer_alloc(0);
    return obj;
}