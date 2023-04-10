cmake_minimum_required(VERSION 3.0.0)
project(cshmem VERSION 0.1.0)

# Create a shared library
add_library(
    shared_mem SHARED 
    src/shared_memory.h
    src/shared_memory.c
)

# Create executable 1
add_executable(writeshm src/writeshmem.c)
target_link_libraries(writeshm shared_mem)

# Create executable 2
add_executable(readshm src/readshmem.c)
target_link_libraries(readshm shared_mem)

# Create executable 3
add_executable(destroyshm src/destroy_shmem.c)
target_link_libraries(destroyshm shared_mem)
