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

#include <ntifs.h>
#include <ntimage.h>
#include <cstdint>

#ifndef NOX_HPP
#define NOX_HPP

// single external import - all we need
extern "C" __declspec(dllimport) LIST_ENTRY* PsLoadedModuleList;

// module entry structure
typedef struct _NOX_MODULE_ENTRY
{
    LIST_ENTRY load_links;
    PVOID exception_table;
    ULONG exception_table_size;
    PVOID global_value;
    PVOID debug_info;
    PVOID image_base;
    PVOID entry_point;
    ULONG image_size;
    UNICODE_STRING full_path;
    UNICODE_STRING base_name;
    ULONG flags;
    USHORT load_counter;
    USHORT padding;
    PVOID section_ptr;
    ULONG checksum;
    PVOID imports;
    PVOID patch_info;
} NOX_MODULE_ENTRY, * PNOX_MODULE_ENTRY;

// main nox class - handles any kernel module
class nox
{
private:
    std::uintptr_t base_address = 0;

    // case-insensitive wide string compare
    __forceinline int wcscmp_ci(const wchar_t* a, const wchar_t* b)
    {
        if (!a || !b) return -1;
        while (*a && *b)
        {
            wchar_t ca = (*a >= L'A' && *a <= L'Z') ? (*a + 0x20) : *a;
            wchar_t cb = (*b >= L'A' && *b <= L'Z') ? (*b + 0x20) : *b;
            if (ca != cb) return ca - cb;
            a++; b++;
        }
        return (*a) ? 1 : (*b) ? -1 : 0;
    }

    // case-sensitive string compare
    __forceinline int strcmp_cs(const char* a, const char* b)
    {
        while (*a && *b)
        {
            if (*a != *b) return *a - *b;
            a++; b++;
        }
        return (*a) ? 1 : (*b) ? -1 : 0;
    }

public:
    // constructor - empty, needs manual init
    __forceinline nox() = default;

    // constructor - auto init with ntoskrnl
    __forceinline nox(bool auto_init)
    {
        if (auto_init && PsLoadedModuleList)
            base_address = reinterpret_cast<std::uintptr_t>(((PNOX_MODULE_ENTRY)PsLoadedModuleList)->image_base);
    }

    // constructor - init with custom module
    __forceinline nox(const wchar_t* module_name)
    {
        base_address = get_module_base(module_name);
    }

    // init with ntoskrnl
    __forceinline bool init()
    {
        if (PsLoadedModuleList)
            base_address = reinterpret_cast<std::uintptr_t>(((PNOX_MODULE_ENTRY)PsLoadedModuleList)->image_base);
        return base_address != 0;
    }

    // init with custom module
    __forceinline bool init(const wchar_t* module_name)
    {
        base_address = get_module_base(module_name);
        return base_address != 0;
    }

    // get module base by name
    __forceinline std::uintptr_t get_module_base(const wchar_t* module_name)
    {
        if (!PsLoadedModuleList) return 0;

        PNOX_MODULE_ENTRY entry = (PNOX_MODULE_ENTRY)PsLoadedModuleList;

        do
        {
            if (entry->base_name.buffer && wcscmp_ci(entry->base_name.buffer, module_name) == 0)
                return reinterpret_cast<std::uintptr_t>(entry->image_base);

            entry = (PNOX_MODULE_ENTRY)entry->load_links.flink;
        } while (entry != (PNOX_MODULE_ENTRY)PsLoadedModuleList);

        return 0;
    }

    // get all modules (returns first entry for iteration)
    __forceinline PNOX_MODULE_ENTRY get_module_list()
    {
        return (PNOX_MODULE_ENTRY)PsLoadedModuleList;
    }

    // get export address from current module
    __forceinline std::uintptr_t get_export(const char* export_name)
    {
        return get_export(base_address, export_name);
    }

    // get export address from specific module
    __forceinline std::uintptr_t get_export(std::uintptr_t module_base, const char* export_name)
    {
        if (!module_base) return 0;

        PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module_base;
        if (dos->e_magic != IMAGE_DOS_SIGNATURE) return 0;

        PIMAGE_NT_HEADERS64 nt_hdr = (PIMAGE_NT_HEADERS64)(module_base + dos->e_lfanew);
        DWORD exports_rva = nt_hdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
        if (!exports_rva) return 0;

        PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)(module_base + exports_rva);
        DWORD* names = (DWORD*)(module_base + exports->AddressOfNames);
        WORD* ordinals = (WORD*)(module_base + exports->AddressOfNameOrdinals);
        DWORD* functions = (DWORD*)(module_base + exports->AddressOfFunctions);

        for (DWORD i = 0; i < exports->NumberOfNames; i++)
        {
            const char* name = (const char*)(module_base + names[i]);
            if (strcmp_cs(name, export_name) == 0)
                return (module_base + functions[ordinals[i]]);
        }
        return 0;
    }

    // dynamic function call from current module
    template<typename Func, typename... Args>
    __forceinline auto call(const char* func_name, Args... f_args)
    {
        std::uintptr_t func_ptr = get_export(func_name);
        if (!func_ptr) return decltype(((Func*)nullptr)(f_args...))();
        return ((Func*)func_ptr)(f_args...);
    }

    // dynamic function call from specific module
    template<typename Func, typename... Args>
    __forceinline auto call(std::uintptr_t module_base, const char* func_name, Args... f_args)
    {
        std::uintptr_t func_ptr = get_export(module_base, func_name);
        if (!func_ptr) return decltype(((Func*)nullptr)(f_args...))();
        return ((Func*)func_ptr)(f_args...);
    }

    // get base address
    __forceinline std::uintptr_t base() const { return base_address; }
    
    // get base address as PVOID (convenience for Windows APIs)
    __forceinline PVOID base_ptr() const { return reinterpret_cast<PVOID>(base_address); }

    // check if module is loaded
    __forceinline explicit operator bool() const { return base_address != 0; }

    // set custom base (for manual override)
    __forceinline void set_base(std::uintptr_t base)
    { 
        base_address = base; 
    }
    
    // set custom base from PVOID
    __forceinline void set_base(PVOID base)
    { 
        base_address = reinterpret_cast<std::uintptr_t>(base); 
    }
};

#endif // NOX_HPP
