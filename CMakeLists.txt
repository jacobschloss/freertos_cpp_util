add_library(freertos_cpp_util

	src/Byte_stream.cpp

	src/FreeRTOS_allocator.cpp

	src/Condition_variable.cpp

	src/Queue_base.cpp
	src/Queue_static.cpp
	src/Queue_static_pod.cpp
	
	src/Semaphore_base.cpp
	src/BSema_static.cpp
	src/CSema_static.cpp

	src/Critical_section_isr.cpp
	src/Critical_section.cpp
	src/Suspend_task_scheduler.cpp
	
	src/Task_base.cpp
	src/Task_heap.cpp
	src/Task_static.cpp

	src/Mutex_base.cpp
	src/Mutex_static.cpp
	src/Mutex_heap.cpp

	src/object_pool/Object_pool.cpp
	src/object_pool/Object_pool_base.cpp
	src/object_pool/Object_pool_node.cpp
)


target_include_directories(freertos_cpp_util PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(freertos_cpp_util
	freertos_v9
)