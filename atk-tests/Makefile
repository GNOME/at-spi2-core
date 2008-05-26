CFLAGS=-I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/atk-1.0

OBJS:=AtkObject_parent_child.o main.o MyAtkAction.o MyAtkComponent.o MyAtkImplementor.o test-simple-child.o test-simple-table.o test-simple-text.c

atktest: $(OBJS)
	$(CC) $(CFLAGS) -o$@ $^ -latk-1.0 -lglib-2.0
