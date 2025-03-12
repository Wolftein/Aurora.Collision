// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2019 by Agustin L. Alvarez. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "Core/Proxy.hpp"
#include <Collision/Grid.hpp>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

inline namespace Proxy
{
    [export, uuid("B46E3A5B-CD15-11EE-ADEF-1418C3A8EDB8")]
    typedef struct UUID
    {
        vbInt32 Name;
        vbInt32 Type;
    } UUID;

    [export, v1_enum]
    typedef enum Grid_Sort
    {
        eGridSortNone,
        eGridSortClosest,
        eGridSortFarthest,
    } Grid_Sort;

    [object, uuid("654649D4-CCFF-11EE-ADEF-1418C3A8EDB8"), pointer_default(unique)]
    __interface Grid_
    {
        HRESULT Initialise([in] vbInt32 Zones);

        HRESULT Attach([in] vbInt32 OnCreate, [in] vbInt32 OnDelete, [in] vbInt32 OnUpdate);

        HRESULT Create([in] UUID * ID, [in] vbInt32 RadiusX, [in] vbInt32 RadiusY, [in] vbInt32 Map, [in] vbInt32 X, [in] vbInt32 Y, [in] vbInt32 Width, [in] vbInt32 Height);

        HRESULT Delete([in] UUID * ID);

        HRESULT Update([in] UUID * ID, [in] vbInt32 Map, [in] vbInt32 X, [in] vbInt32 Y, [in] vbBool Warp);

        HRESULT Search([in] UUID * ID, [in] vbInt32 Type, [in] Grid_Sort Sort, [in, satype(UUID)] SAFEARRAY ** Result);

        HRESULT Query([in] UUID * ID, [in] vbInt32 Type, [in] Grid_Sort Sort, [in, satype(UUID)] SAFEARRAY ** Result);

        HRESULT QueryAt([in] vbInt32 Map, [in] vbInt32 X, [in] vbInt32 Y, [in] vbInt32 Distance, [in] vbInt32 Type, [in] Grid_Sort Sort, [in, satype(UUID)] SAFEARRAY ** Result);
    };

    [coclass, uuid("68CEDDA0-CCFF-11EE-ADEF-1418C3A8EDB8")]
    class ATL_NO_VTABLE Grid : public Grid_
    {
    public:

        // @see Grid::FinalConstruct
        HRESULT FinalConstruct();

        // @see Grid::FinalRelease
        HRESULT FinalRelease();

        // @see Grid_::Initialise
        HRESULT Initialise(vbInt32 Zones);

        // @see Grid_::Attach
        HRESULT Attach(vbInt32 OnCreate, vbInt32 OnDelete, vbInt32 OnUpdate);

        // @see Grid_::Create
        HRESULT Create(UUID * ID, vbInt32 RadiusX, vbInt32 RadiusY, vbInt32 Map, vbInt32 X, vbInt32 Y, vbInt32 Width, vbInt32 Height);

        // @see Grid_::Delete
        HRESULT Delete(UUID * ID);

        // @see Grid_::Update
        HRESULT Update(UUID * ID, vbInt32 Map, vbInt32 X, vbInt32 Y, vbBool Warp);

        // @see Grid_::Search
        HRESULT Search(UUID * ID, vbInt32 Type, Grid_Sort Sort, SAFEARRAY ** Result);

        // @see Grid_::Query
        HRESULT Query(UUID * ID, vbInt32 Type, Grid_Sort Sort, SAFEARRAY ** Result);

        // @see Grid_::QueryAt
        HRESULT QueryAt(vbInt32 Map, vbInt32 X, vbInt32 Y, vbInt32 Distance, vbInt32 Type, Grid_Sort Sort, SAFEARRAY ** Result);

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        UPtr<Aurora::Collision::Grid> mWrapper;

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Vector<UUID>                  mFiltered;
    };
}