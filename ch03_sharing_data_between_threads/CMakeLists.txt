project(ch3)

add_executable(03_01_mutex 01_mutex.cc)

add_executable(03_02_bypass_mutex 02_bypass_mutex.cc)

add_executable(03_03_thread_safe_stack 03_thread_safe_stack.cc threadsafe_stack.h)

add_executable(03_04_avoid_deadlock 04_avoid_deadlock.cc)

add_executable(03_05_lock_hierarchy 05_lock_hierarchy.cc hierarchical_mutex.h)
