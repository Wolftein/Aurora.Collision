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

#include <EASTL/hash_map.h>
#include <EASTL/priority_queue.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/span.h>
#include <EASTL/string.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/vector.h>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

inline namespace STL
{
    // -=(Undocumented)=-
    using Bool   = bool8_t;

    // -=(Undocumented)=-
    using SInt08 = int8_t;

    // -=(Undocumented)=-
    using SInt16 = int16_t;

    // -=(Undocumented)=-
    using SInt32 = int32_t;

    // -=(Undocumented)=-
    using SInt64 = int64_t;

	// -=(Undocumented)=-
	using UInt08 = uint8_t;

	// -=(Undocumented)=-
	using UInt16 = uint16_t;

	// -=(Undocumented)=-
	using UInt32 = uint32_t;

	// -=(Undocumented)=-
	using UInt64 = uint64_t;

    // -=(Undocumented)=-
    using Real32 = float_t;

    // -=(Undocumented)=-
    using Real64 = double_t;

    // -=(Undocumented)=-
    using Str8   = eastl::string;

    // -=(Undocumented)=-
    using Str16  = eastl::wstring;

    // -=(Undocumented)=-
    using CStr8  = eastl::string_view;

    // -=(Undocumented)=-
    using CStr16 = eastl::wstring_view;

    // -=(Undocumented)=-
    template<typename T>
    using CPtr   = eastl::span<T>;

    // -=(Undocumented)=-
    template<typename T>
    using SPtr   = eastl::shared_ptr<T>;

    // -=(Undocumented)=-
    template<typename T>
    using UPtr   = eastl::unique_ptr<T>;

    // -=(Undocumented)=-
    template<typename T>
    using Vector = eastl::vector<T>;

    // -=(Undocumented)=-
    template<typename T, typename J>
    using Table  = eastl::hash_map<T, J>;

    // -=(Undocumented)=-
    template<typename T>
    using Method = eastl::function<T>;
}
