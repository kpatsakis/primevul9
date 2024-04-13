static void add_tag_extraction_to_list (char *spec)
{
    tag_extractions = realloc (tag_extractions, (num_tag_extractions + 1) * sizeof (*tag_extractions));
    tag_extractions [num_tag_extractions] = malloc (strlen (spec) + 10);
    strcpy (tag_extractions [num_tag_extractions], spec);
    num_tag_extractions++;
}