add_library(freertos_cpp_util

	src/Byte_stream.cpp

	src/FreeRTOS_allocator.cpp

	src/Call_once.cpp

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
	
	src/util/Alloc_inplace.cpp

	src/Task_base.cpp
	src/Task_heap.cpp
	src/Task_static.cpp

	src/Mutex_base.cpp
	src/Mutex_heap.cpp
	src/Mutex_static.cpp
	src/Mutex_static_recursive.cpp

	src/Stream_buffer.cpp

	src/object_pool/Object_pool.cpp
	src/object_pool/Object_pool_base.cpp
	src/object_pool/Object_pool_node.cpp

	src/logging/Global_logger.cpp
	src/logging/Log_sink_base.cpp
	src/logging/Log_sink_console.cpp
	src/logging/Logger.cpp
	src/logging/Logger_types.cpp
)


target_include_directories(freertos_cpp_util PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(freertos_cpp_util
	common_util
	freertos_v10
)

if(DEFINED Doxygen::doxygen)
	doxygen_add_docs(freertos_cpp_util_docs
		include/
		src/
	)
endif()
