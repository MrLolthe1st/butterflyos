#include "../includes/storage/drives.h"

int name_compare(struct rb_tree * z, struct rb_node * a, struct rb_node * b)
{
	return strcmp((const char *)a->value, (const char *)b->value);
}
#define dnames(f) device_names->operator[]((char*)f)
map<char *, void *> * device_names;
void drives_init()
{
	device_names = (map<char *, void *>*)malloc(sizeof(map<char *, void *>));
	device_names->set_cmp(name_compare);
	dnames("abc") = (void*)111;
	printf("%d", dnames("abc"));
}