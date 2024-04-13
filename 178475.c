static umode_t acrn_attr_visible(struct kobject *kobj, struct attribute *a, int n)
{
       if (a == &dev_attr_remove_cpu.attr)
               return IS_ENABLED(CONFIG_HOTPLUG_CPU) ? a->mode : 0;

       return a->mode;
}