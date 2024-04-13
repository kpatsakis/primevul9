void js_dumpobject(js_State *J, js_Object *obj)
{
	minify = 0;
	printf("{\n");
	if (obj->properties->level)
		js_dumpproperty(J, obj->properties);
	printf("}\n");
}