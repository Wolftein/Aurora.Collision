// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2020 by Agustin Alvarez. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#define _ATL_ATTRIBUTES 1
#include <atldef.h>
#include <atlsafe.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlconv.h>
#include <atlplus.h>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include <Core/Common.hpp>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

inline namespace Proxy
{
    // -=(Undocumented)=-
    [export]
    typedef void *          vbAny;

    // -=(Undocumented)=-
    [export]
    typedef VARIANT_BOOL    vbBool;

    // -=(Undocumented)=-
    [export]
    typedef unsigned __int8 vbInt8;

    // -=(Undocumented)=-
    [export]
    typedef __int16         vbInt16;

    // -=(Undocumented)=-
    [export]
    typedef __int32         vbInt32;

    // -=(Undocumented)=-
    [export]
    typedef VARIANT         vbInt64;

    // -=(Undocumented)=-
    [export]
    typedef float           vbReal32;

    // -=(Undocumented)=-
    [export]
    typedef double          vbReal64;

    // -=(Undocumented)=-
    [export]
    typedef LPCSTR          vbStr8;

    // -=(Undocumented)=-
    [export]
    typedef BSTR            vbStr16;

    // -=(Undocumented)=-
#define VBTrue              VARIANT_TRUE

    // -=(Undocumented)=-
#define VBFalse             VARIANT_FALSE

    // -=(Undocumented)=-
    inline auto VBString16ToString8(vbStr16 Value)
    {
        const auto Length = SysStringLen(Value);
        Str8 Result(WideCharToMultiByte(CP_UTF8, 0, Value, Length, nullptr, 0, nullptr, nullptr), '\0');
        WideCharToMultiByte(CP_UTF8, 0, Value, Length, Result.data(), Result.size(), nullptr, nullptr);
        return Result;
    }

    // -=(Undocumented)=-
    inline auto VBString8ToString16(CStr8 Value)
    {
        auto Result = SysAllocStringLen(
            nullptr, MultiByteToWideChar(CP_UTF8, 0, Value.data(), Value.size(), nullptr, 0));
        MultiByteToWideChar(CP_UTF8, 0, Value.data(), Value.size(), Result, Value.size());
        return Result;
    }

    // -=(Undocumented)=-
    template<typename Type>
    inline auto VBSpanToSafeArray(CPtr<Type> Chunk, SAFEARRAY ** Result)
    {
        static SAFEARRAY Unique; // @NOTE: one array at a time (this prevent allocation, BIG BONUS)

        Unique.cDims      = 1;
        Unique.fFeatures  = FADF_AUTO | FADF_FIXEDSIZE;
        Unique.cbElements = sizeof(Type);
        Unique.cLocks     = 1;
        Unique.pvData     = static_cast<LPVOID>(Chunk.data());

        Unique.rgsabound[0].lLbound   = 0;
        Unique.rgsabound[0].cElements = Chunk.size();

        (* Result) = & Unique;
    }

    // -=(Undocumented)=-
    inline auto VBSafeArrayToSpan(SAFEARRAY * Result)
    {
        UInt32 Count = Result->cbElements;

        for (UInt32 Index = 0; Index < Result->cDims; ++Index)
        {
            Count *= Result->rgsabound[Index].cElements;
        }
        return CPtr<UInt08>(static_cast<UInt08 *>(Result->pvData), Count);
    }
}