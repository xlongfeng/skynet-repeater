set(SRC
	portable/GCC/ARM_CM3/port.c
	portable/MemMang/heap_4.c
	event_groups.c
	list.c
	queue.c
	stream_buffer.c
	tasks.c
	timers.c
	)

add_library(freertos STATIC ${SRC})