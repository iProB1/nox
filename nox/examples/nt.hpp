/*
    MIT License

    Copyright (c) 2026 ProB1

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

#include "nox.hpp"

#ifndef NT_HPP
#define NT_HPP

// specialized nt class for ntoskrnl operations
class nox_nt : public nox
{
public:
    // constructor - auto init with ntoskrnl
    __forceinline nox_nt() : nox(true) {}

    // ================ WRAPPER FUNCTIONS ================

    // process functions
    __forceinline PVOID ps_get_current_process()
    {
        return call<PVOID(*)()>("PsGetCurrentProcess");
    }

    __forceinline PVOID ps_get_process_peb(PVOID eprocess)
    {
        return call<PVOID(*)(PVOID)>("PsGetProcessPeb", eprocess);
    }

    __forceinline ULONG ps_get_process_session_id(PVOID eprocess)
    {
        return call<ULONG(*)(PVOID)>("PsGetProcessSessionId", eprocess);
    }

    __forceinline const char* ps_get_process_image_file_name(PVOID eprocess)
    {
        return call<const char* (*)(PVOID)>("PsGetProcessImageFileName", eprocess);
    }

    __forceinline ULONG64 ps_get_process_section_base_address(PVOID eprocess)
    {
        return call<ULONG64(*)(PVOID)>("PsGetProcessSectionBaseAddress", eprocess);
    }

    __forceinline NTSTATUS ps_lookup_process_by_pid(HANDLE pid, PVOID* process)
    {
        return call<NTSTATUS(*)(HANDLE, PVOID*)>("PsLookupProcessByProcessId", pid, process);
    }

    __forceinline NTSTATUS ps_lookup_thread_by_tid(HANDLE tid, PVOID* thread)
    {
        return call<NTSTATUS(*)(HANDLE, PVOID*)>("PsLookupThreadByThreadId", tid, thread);
    }

    __forceinline VOID ob_dereference_object(PVOID object)
    {
        call<VOID(*)(PVOID)>("ObfDereferenceObject", object);
    }

    // memory functions
    __forceinline BOOLEAN mm_is_address_valid(PVOID address)
    {
        return call<BOOLEAN(*)(PVOID)>("MmIsAddressValid", address);
    }

    __forceinline PVOID mm_get_system_routine_address(PUNICODE_STRING routine_name)
    {
        return call<PVOID(*)(PUNICODE_STRING)>("MmGetSystemRoutineAddress", routine_name);
    }

    __forceinline PHYSICAL_ADDRESS mm_get_physical_address(PVOID virtual_address)
    {
        return call<PHYSICAL_ADDRESS(*)(PVOID)>("MmGetPhysicalAddress", virtual_address);
    }

    __forceinline PVOID mm_map_io_space(PHYSICAL_ADDRESS physical_address, SIZE_T number_of_bytes, MEMORY_CACHING_TYPE cache_type)
    {
        return call<PVOID(*)(PHYSICAL_ADDRESS, SIZE_T, MEMORY_CACHING_TYPE)>("MmMapIoSpace", physical_address, number_of_bytes, cache_type);
    }

    __forceinline VOID mm_unmap_io_space(PVOID base_address, SIZE_T number_of_bytes)
    {
        call<VOID(*)(PVOID, SIZE_T)>("MmUnmapIoSpace", base_address, number_of_bytes);
    }

    __forceinline NTSTATUS mm_copy_memory(PVOID target_address, MM_COPY_ADDRESS source_address, SIZE_T number_of_bytes, ULONG flags, PSIZE_T number_of_bytes_transferred)
    {
        return call<NTSTATUS(*)(PVOID, MM_COPY_ADDRESS, SIZE_T, ULONG, PSIZE_T)>("MmCopyMemory", target_address, source_address, number_of_bytes, flags, number_of_bytes_transferred);
    }

    // pool functions
    __forceinline PVOID ex_allocate_pool(POOL_TYPE pool_type, SIZE_T size)
    {
        return call<PVOID(*)(POOL_TYPE, SIZE_T)>("ExAllocatePool", pool_type, size);
    }

    __forceinline VOID ex_free_pool(PVOID pool)
    {
        call<VOID(*)(PVOID)>("ExFreePool", pool);
    }

    // resource functions
    __forceinline BOOLEAN ex_acquire_resource_exclusive(PVOID resource, BOOLEAN wait)
    {
        return call<BOOLEAN(*)(PVOID, BOOLEAN)>("ExAcquireResourceExclusiveLite", resource, wait);
    }

    __forceinline VOID ex_release_resource(PVOID resource)
    {
        call<VOID(*)(PVOID)>("ExReleaseResourceLite", resource);
    }

    // string functions
    __forceinline VOID rtl_init_unicode_string(PUNICODE_STRING dest, PCWSTR src)
    {
        call<VOID(*)(PUNICODE_STRING, PCWSTR)>("RtlInitUnicodeString", dest, src);
    }

    __forceinline BOOLEAN rtl_equal_unicode_string(PUNICODE_STRING s1, PUNICODE_STRING s2, BOOLEAN case_insensitive)
    {
        return call<BOOLEAN(*)(PUNICODE_STRING, PUNICODE_STRING, BOOLEAN)>("RtlEqualUnicodeString", s1, s2, case_insensitive);
    }

    __forceinline LONG rtl_compare_unicode_string(PUNICODE_STRING s1, PUNICODE_STRING s2, BOOLEAN case_insensitive)
    {
        return call<LONG(*)(PUNICODE_STRING, PUNICODE_STRING, BOOLEAN)>("RtlCompareUnicodeString", s1, s2, case_insensitive);
    }

    // thread functions
    __forceinline VOID ke_initialize_dpc(PKDPC dpc, PKDEFERRED_ROUTINE routine, PVOID context)
    {
        call<VOID(*)(PKDPC, PKDEFERRED_ROUTINE, PVOID)>("KeInitializeDpc", dpc, routine, context);
    }

    __forceinline BOOLEAN ke_insert_queue_dpc(PKDPC dpc, PVOID system_arg1, PVOID system_arg2)
    {
        return call<BOOLEAN(*)(PKDPC, PVOID, PVOID)>("KeInsertQueueDpc", dpc, system_arg1, system_arg2);
    }

    __forceinline NTSTATUS ke_set_target_processor_dpc_ex(PKDPC dpc, PPROCESSOR_NUMBER processor_number)
    {
        return call<NTSTATUS(*)(PKDPC, PPROCESSOR_NUMBER)>("KeSetTargetProcessorDpcEx", dpc, processor_number);
    }

    __forceinline ULONG ke_query_maximum_processor_count()
    {
        return call<ULONG(*)()>("KeQueryMaximumProcessorCount");
    }

    __forceinline KAFFINITY ke_query_active_processors()
    {
        return call<KAFFINITY(*)()>("KeQueryActiveProcessors");
    }

    __forceinline VOID ke_set_system_affinity_thread(KAFFINITY affinity)
    {
        call<VOID(*)(KAFFINITY)>("KeSetSystemAffinityThread", affinity);
    }

    __forceinline VOID ke_restore_system_affinity_thread()
    {
        call<VOID(*)()>("KeRevertToUserAffinityThread");
    }

    // attach functions
    __forceinline VOID ke_stack_attach_process(PEPROCESS process, PKAPC_STATE apc_state)
    {
        call<VOID(*)(PEPROCESS, PKAPC_STATE)>("KeStackAttachProcess", process, apc_state);
    }

    __forceinline VOID ke_unstack_detach_process(PKAPC_STATE apc_state)
    {
        call<VOID(*)(PKAPC_STATE)>("KeUnstackDetachProcess", apc_state);
    }

    // system information
    __forceinline NTSTATUS zw_query_system_information(ULONG info_class, PVOID buffer, ULONG buffer_size, PULONG return_length)
    {
        return call<NTSTATUS(*)(ULONG, PVOID, ULONG, PULONG)>("ZwQuerySystemInformation", info_class, buffer, buffer_size, return_length);
    }

    __forceinline NTSTATUS zw_close(HANDLE handle)
    {
        return call<NTSTATUS(*)(HANDLE)>("ZwClose", handle);
    }

    // memory operations
    __forceinline VOID memcpy(PVOID dst, PVOID src, SIZE_T size)
    {
        auto fn = get_export("memcpy");
        if (fn) ((VOID(*)(PVOID, PVOID, SIZE_T))fn)(dst, src, size);
    }

    __forceinline VOID memset(PVOID dst, int value, SIZE_T size)
    {
        auto fn = get_export("memset");
        if (fn) ((VOID(*)(PVOID, int, SIZE_T))fn)(dst, value, size);
    }

    // TLB and cache operations
    __forceinline VOID ke_flush_entire_tb(BOOLEAN invalidate, BOOLEAN all_processors)
    {
        call<VOID(*)(BOOLEAN, BOOLEAN)>("KeFlushEntireTb", invalidate, all_processors);
    }

    __forceinline VOID ke_invalidate_all_caches()
    {
        call<VOID(*)()>("KeInvalidateAllCaches");
    }

    __forceinline VOID ke_flush_single_tb(ULONG_PTR address, BOOLEAN all_processors, BOOLEAN invalidate)
    {
        call<VOID(*)(ULONG_PTR, BOOLEAN, BOOLEAN)>("KeFlushSingleTb", address, all_processors, invalidate);
    }

    // combined cache flush
    __forceinline VOID flush_caches(PVOID address)
    {
        ke_flush_entire_tb(TRUE, TRUE);
        ke_invalidate_all_caches();
        ke_flush_single_tb((ULONG_PTR)address, TRUE, TRUE);
    }

    // processor functions
    __forceinline ULONG ke_get_current_processor_number()
    {
        return call<ULONG(*)()>("KeGetCurrentProcessorNumber");
    }

    // interlocked operations
    __forceinline LONG interlocked_exchange(PLONG destination, LONG value)
    {
        return _InterlockedExchange(destination, value);
    }

    __forceinline LONG64 interlocked_exchange64(PLONG64 destination, LONG64 value)
    {
        return _InterlockedExchange64(destination, value);
    }

    __forceinline PVOID interlocked_exchange_pointer(PVOID* destination, PVOID value)
    {
        return _InterlockedExchangePointer(destination, value);
    }

    __forceinline LONG interlocked_compare_exchange(PLONG destination, LONG exchange, LONG comperand)
    {
        return _InterlockedCompareExchange(destination, exchange, comperand);
    }

    __forceinline LONG64 interlocked_compare_exchange64(PLONG64 destination, LONG64 exchange, LONG64 comperand)
    {
        return _InterlockedCompareExchange64(destination, exchange, comperand);
    }
};

inline nox_nt nt;

#define NT_CALL(func, ...) nt.call<decltype(func)*>(#func, __VA_ARGS__)
#define NT_EXPORT(name) nt.get_export(name)
#define NT_MODULE(name) nt.get_module_base(name)

#endif // NT_HPP
