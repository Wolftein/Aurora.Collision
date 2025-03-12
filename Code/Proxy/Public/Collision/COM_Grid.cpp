// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2019 by Agustin L. Alvarez. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "COM_Grid.hpp"
#include <algorithm>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

inline namespace Proxy
{
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    static UInt32 AsID(const UUID & ID)
    {
        return (static_cast<UInt08>(ID.Type) << 24) | (ID.Name & 0x00FFFFFF);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    static UUID  AsUUID(UInt32 ID)
    {
        return { static_cast<vbInt32>(ID & 0x00FFFFFF), static_cast<vbInt32>(ID >> 24) & 0xFF };
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    HRESULT Grid::FinalConstruct()
    {
        return S_OK;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    HRESULT Grid::FinalRelease()
    {
        mWrapper = nullptr;
        return S_OK;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    HRESULT Grid::Initialise(vbInt32 Zones)
    {
        mWrapper = eastl::make_unique<Aurora::Collision::Grid>(Zones);
        return S_OK;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    HRESULT Grid::Attach(vbInt32 OnCreate, vbInt32 OnDelete, vbInt32 OnUpdate)
    {
        const auto OnGridCreate = [OnCreate](UInt32 Entity, UInt32 Observer) {
            UUID UUID_1 = AsUUID(Entity);
            UUID UUID_2 = AsUUID(Observer);
            ((void (STDAPICALLTYPE *)(UUID *, UUID *)) OnCreate)(& UUID_1, & UUID_2);
        };
        const auto OnGridDelete = [OnDelete](UInt32 Entity, UInt32 Observer) {
            UUID UUID_1 = AsUUID(Entity);
            UUID UUID_2 = AsUUID(Observer);
            ((void (STDAPICALLTYPE *)(UUID *, UUID *)) OnDelete)(& UUID_1, & UUID_2);
        };
        const auto OnGridUpdate = [OnUpdate](UInt32 Entity, UInt32 Observer, Bool Warped) {
            UUID UUID_1 = AsUUID(Entity);
            UUID UUID_2 = AsUUID(Observer);
            ((void (STDAPICALLTYPE *)(UUID *, UUID *, vbBool)) OnUpdate)(& UUID_1, & UUID_2, Warped ? VBTrue : VBFalse);
        };

        mWrapper->Attach(OnGridCreate, OnGridDelete, OnGridUpdate);
        return S_OK;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    HRESULT Grid::Create(UUID * ID, vbInt32 RadiusX, vbInt32 RadiusY, vbInt32 Map, vbInt32 X, vbInt32 Y, vbInt32 Width, vbInt32 Height)
    {
        mWrapper->Create(AsID(* ID), RadiusX, RadiusY, Map, X, Y, Width, Height);
        return S_OK;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    HRESULT Grid::Delete(UUID * ID)
    {
        mWrapper->Delete(AsID(* ID));
        return S_OK;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    HRESULT Grid::Update(UUID * ID, vbInt32 Map, vbInt32 X, vbInt32 Y, vbBool Warp)
    {
        mWrapper->Update(AsID(* ID), Map, X, Y, (Warp == VBTrue));
        return S_OK;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    HRESULT Grid::Search(UUID * ID, vbInt32 Type, Grid_Sort Sort, SAFEARRAY ** Result)
    {
        mFiltered.clear();

        if (Type == UINT8_MAX)
        {
            for (const UInt32 Instigator : mWrapper->Query(AsID(* ID), (Aurora::Collision::Sort) Sort))
            {
                mFiltered.emplace_back(AsUUID(Instigator));
            }
        }
        else
        {
            for (const UInt32 Instigator : mWrapper->Query(AsID(* ID), (Aurora::Collision::Sort) Sort))
            {
                const UUID UUID = AsUUID(Instigator);

                if (UUID.Type == Type)
                {
                    mFiltered.emplace_back(UUID);
                }
            }
        }

        VBSpanToSafeArray(CPtr<UUID>(mFiltered), Result);
        return S_OK;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    HRESULT Grid::Query(UUID * ID, vbInt32 Type, Grid_Sort Sort, SAFEARRAY ** Result)
    {
        mFiltered.clear();

        if (Type == UINT8_MAX)
        {
            for (const UInt32 Instigator : mWrapper->QueryObserver(AsID(* ID), (Aurora::Collision::Sort) Sort))
            {
                mFiltered.emplace_back(AsUUID(Instigator));
            }
        }
        else
        {
            for (const UInt32 Instigator : mWrapper->QueryObserver(AsID(* ID), (Aurora::Collision::Sort) Sort))
            {
                const UUID UUID = AsUUID(Instigator);

                if (UUID.Type == Type)
                {
                    mFiltered.emplace_back(UUID);
                }
            }
        }

        VBSpanToSafeArray(CPtr<UUID>(mFiltered), Result);
        return S_OK;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    HRESULT Grid::QueryAt(vbInt32 Map, vbInt32 X, vbInt32 Y, vbInt32 Distance, vbInt32 Type, Grid_Sort Sort, SAFEARRAY ** Result)
    {
        mFiltered.clear();

        if (Type == UINT8_MAX)
        {
            for (const UInt32 Instigator : mWrapper->QueryAt(Map, X, Y, Distance, (Aurora::Collision::Sort) Sort))
            {
                mFiltered.emplace_back(AsUUID(Instigator));
            }
        }
        else
        {
            for (const UInt32 Instigator : mWrapper->QueryAt(Map, X, Y, Distance, (Aurora::Collision::Sort) Sort))
            {
                const UUID UUID = AsUUID(Instigator);

                if (UUID.Type == Type)
                {
                    mFiltered.emplace_back(UUID);
                }
            }
        }

        VBSpanToSafeArray(CPtr<UUID>(mFiltered), Result);
        return S_OK;
    }
}