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

#include "Entity.hpp"
#include "Group.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   DATA   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Aurora::Collision
{
    constexpr static SInt32 kChunkAreaX  = 100; // @TODO: Remove hardcoded value
    constexpr static SInt32 kChunkAreaY  = 100; // @TODO: Remove hardcoded value
    constexpr static SInt32 kChunkRadius = 25;  // @TODO: Remove hardcoded value
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Aurora::Collision
{
    // -=(Undocumented)=-
    enum class Sort
    {
        None,
        Closest,
        Farthest,
    };

    // -=(Undocumented)=-
    class Grid
    {
    public:

        // -=(Undocumented)=-
        using OnCreate = Method<void(UInt32 Entity, UInt32 Observer)>;

        // -=(Undocumented)=-
        using OnDelete = Method<void(UInt32 Entity, UInt32 Observer)>;

        // -=(Undocumented)=-
        using OnUpdate = Method<void(UInt32 Entity, UInt32 Observer, Bool Warped)>;

    public:

        // -=(Undocumented)=-
        Grid(UInt32 Zones);

        // -=(Undocumented)=-
        void Attach(OnCreate OnCreate, OnDelete OnDelete, OnUpdate OnUpdate);

        // -=(Undocumented)=-
        void Create(UInt32 ID, SInt16 RadiusX, SInt16 RadiusY, SInt16 Map, SInt32 X, SInt32 Y, SInt32 Width, SInt32 Height);

        // -=(Undocumented)=-
        void Delete(UInt32 ID);

        // -=(Undocumented)=-
        void Update(UInt32 ID, SInt16 Map, SInt32 X, SInt32 Y, Bool Warped);

        // -=(Undocumented)=-
        CPtr<UInt32> Query(UInt32 ID, Sort Sorting);

        // -=(Undocumented)=-
        CPtr<UInt32> QueryObserver(UInt32 ID, Sort Sorting);

        // -=(Undocumented)=-
        CPtr<UInt32> QueryAt(SInt32 Map, SInt32 X, SInt32 Y, SInt32 Distance, Sort Sorting);

        // -=(Undocumented)=-
        Entity * GetEntity(UInt32 ID)
        {
            const auto Iterator = mEntities.find(ID);
            return (Iterator != mEntities.end() ? & Iterator->second : nullptr);
        }

    private:

        // -=(Undocumented)=-
        struct Cell
        {
            Vector<Entity *> Bodies;
            Vector<Entity *> Observers;
        };

        // -=(Undocumented)=-
        void CreateEntity(Entity & Instigator);

        // -=(Undocumented)=-
        void DeleteEntity(Entity & Instigator);

        // -=(Undocumented)=-
        void UpdateEntity(Entity & Instigator, const Rectangle & Destination, Bool Warped);

        // -=(Undocumented)=-
        void CreateOnCell(Cell & Cell, Entity & Instigator);

        // -=(Undocumented)=-
        void DeleteOnCell(Cell & Cell, Entity & Instigator);

        // -=(Undocumented)=-
        void UpdateOnCell(Cell & Cell, Entity & Instigator, Bool Warped, Group & Filter);

        // -=(Undocumented)=-
        void UpdateOrCreateOnCell(Cell & Cell, Entity & Instigator, Bool Warped, Group & Filter);

        // -=(Undocumented)=-
        void RemoveOnCell(Cell & Cell, Entity & Instigator);

        // -=(Undocumented)=-
        void SearchOnCell(Cell & Cell, Entity & Instigator, Group & Filter);

        // -=(Undocumented)=-
        void CreateViewerOnCell(Cell & Cell, Entity & Instigator);

        // -=(Undocumented)=-
        void DeleteViewerOnCell(Cell & Cell, Entity & Instigator);

        // -=(Undocumented)=-
        void UpdateViewerOnCell(Cell & Cell, Entity & Instigator);

        // -=(Undocumented)=-
        void CreateOnObserver(const Entity & Instigator, Entity & Observer);

        // -=(Undocumented)=-
        void DeleteOnObserver(const Entity & Instigator, Entity & Observer);

        // -=(Undocumented)=-
        void UpdateOnObserver(const Entity & Instigator, Entity & Observer, Bool Warped);

        // -=(Undocumented)=-
        void RemoveOnObserver(const Entity & Instigator, Entity & Observer);

        // -=(Undocumented)=-
        Bool SearchOnObserver(const Entity & Instigator, Entity & Observer);

        // -=(Undocumented)=-
        auto GetCell(SInt16 Map, SInt32 X, SInt32 Y) -> eastl::add_reference<Cell>::type
        {
            constexpr SInt32 SizeX = kChunkAreaX / kChunkRadius;
            constexpr SInt32 SizeY = kChunkAreaY / kChunkRadius;

            return mChunks[(Map * SizeX * SizeY) + (Y * SizeX) + X];
        }

        // -=(Undocumented)=-
        auto GetCoordinates(const Rectangle & Area)
        {
            constexpr SInt32 MinX = 0;
            constexpr SInt32 MinY = 0;
            constexpr SInt32 MaxX = kChunkAreaX / kChunkRadius - 1;
            constexpr SInt32 MaxY = kChunkAreaY / kChunkRadius - 1;

            return Rectangle {
                eastl::max(MinX, Area.GetX1() / kChunkRadius),
                eastl::max(MinY, Area.GetY1() / kChunkRadius),
                eastl::min(MaxX, Area.GetX2() / kChunkRadius),
                eastl::min(MaxY, Area.GetY2() / kChunkRadius)
            };
        }

    private:

        // -=(Undocumented)=-
        void SortQuery(Vector<UInt32> & List, SInt32 X, SInt32 Y, Sort Sorting);


    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Vector<Cell>          mChunks;

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Table<UInt32, Entity> mEntities;
        Table<UInt32, Group>  mObservations;

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        OnCreate              mOnCreate;
        OnDelete              mOnDelete;
        OnUpdate              mOnUpdate;

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Group                 mResult;
    };
}