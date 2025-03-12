// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2020 by Agustin Alvarez. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "Grid.hpp"
#include <windows.h>
#include <algorithm>

#define MY_PRINTF(...) {char cad[512]; sprintf(cad, __VA_ARGS__);  printf("%s\n", cad);}

namespace Aurora::Collision
{
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#define FOR_EACH_ZONE(Function, Area, Entity)                               \
    for (SInt32 ZoneX = Area.GetX1(); ZoneX <= Area.GetX2(); ++ZoneX)       \
        for (SInt32 ZoneY = Area.GetY1(); ZoneY <= Area.GetY2(); ++ZoneY)   \
            Function(GetCell(Entity.GetMap(), ZoneX, ZoneY), Entity)

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Grid::Grid(UInt32 Zones)
    {
        mChunks.resize(Zones * ((kChunkAreaX / kChunkRadius) * (kChunkAreaY / kChunkRadius)));
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::Attach(OnCreate OnCreate, OnDelete OnDelete, OnUpdate OnUpdate)
    {
        mOnCreate = OnCreate;
        mOnDelete = OnDelete;
        mOnUpdate = OnUpdate;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::Create(UInt32 ID, SInt16 RadiusX, SInt16 RadiusY, SInt16 Map, SInt32 X, SInt32 Y, SInt32 Width, SInt32 Height)
    {
        //MY_PRINTF("%d: World.Create(%d, %d, %d, %d, %d, %d, %d)", ID, RadiusX, RadiusY, Map, X, Y, Width, Height);

        const Rectangle Position {
            X - Width,
            Y - Height,
            X + Width,
            Y
        };

        if (const auto & Pair = mEntities.try_emplace(ID, ID, Map, RadiusX, RadiusY, Position); Pair.second)
        {
            CreateEntity(Pair.first->second);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::Delete(UInt32 ID)
    {
        //MY_PRINTF("%d: World.Delete", ID);

        if (const auto Iterator = mEntities.find(ID); Iterator != mEntities.end())
        {
            DeleteEntity(Iterator->second);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::Update(UInt32 ID, SInt16 Map, SInt32 X, SInt32 Y, Bool Warped)
    {
        //MY_PRINTF("World.Update(%d, %d, %d, %d, %s);", ID, Map, X, Y, Warped ? "true" : "false");

        if (const auto Iterator = mEntities.find(ID); Iterator != mEntities.end())
        {
            Entity & Instigator = Iterator->second;
            if (Iterator->second.GetMap() != Map)
            {
                const Rectangle Body = GetCoordinates(Instigator.GetBody());

                FOR_EACH_ZONE(DeleteOnCell, Body, Instigator);

                if (Instigator.GetHeadX() || Instigator.GetHeadY())
                {
                    const Rectangle Head = GetCoordinates(Rectangle(Instigator.GetBody(), Instigator.GetHeadX(), Instigator.GetHeadY()));

                    FOR_EACH_ZONE(DeleteViewerOnCell, Head, Instigator);

                    mObservations.erase(Instigator.GetID());
                }
            }

            const SInt32 Width  = Instigator.GetBody().GetWidth() / 2;
            const SInt32 Height = Instigator.GetBody().GetHeight();

            const Rectangle Position {
                X - Width,
                Y - Height,
                X + Width,
                Y
            };

            UpdateEntity(Instigator, Position, Warped);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    CPtr<UInt32> Grid::Query(UInt32 ID, Sort Sorting)
    {
        CPtr<UInt32> Chunk;

        if (const auto Iterator = mObservations.find(ID); Iterator != mObservations.end())
        {
            if (Sorting != Sort::None)
            {
                mResult.Copy(Iterator->second);

                Entity & Entity = mEntities.find(ID)->second;
                SortQuery(mResult.GetData(), Entity.GetBody().GetCenterX(), Entity.GetBody().GetCenterY(), Sorting);

                Chunk = mResult.GetData();
            }
            else
            {
                Chunk = Iterator->second.GetData();
            }
        }
        return Chunk;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    CPtr<UInt32> Grid::QueryObserver(UInt32 ID, Sort Sorting)
    {
        mResult.Clear();

        if (const auto Iterator = mEntities.find(ID); Iterator != mEntities.end())
        {
            const Rectangle Body = GetCoordinates(Iterator->second.GetBody());

            for (SInt32 ZoneX = Body.GetX1(); ZoneX <= Body.GetX2(); ++ZoneX)
            {
                for (SInt32 ZoneY = Body.GetY1(); ZoneY <= Body.GetY2(); ++ZoneY)
                {
                    SearchOnCell(GetCell(Iterator->second.GetMap(), ZoneX, ZoneY), Iterator->second, mResult);
                }
            }

            if (Sorting != Sort::None)
            {
                SortQuery(mResult.GetData(),
                    Iterator->second.GetBody().GetCenterX(), Iterator->second.GetBody().GetCenterY(), Sorting);
            }
        }

        return mResult.GetData();
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    CPtr<UInt32> Grid::QueryAt(SInt32 Map, SInt32 X, SInt32 Y, SInt32 Distance, Sort Sorting)
    {
        mResult.Clear();

        Entity Dummy(-1, Map, Distance, Distance, Rectangle { X, Y, X, Y});

        const Rectangle Body = GetCoordinates(Dummy.GetBody());

        for (SInt32 ZoneX = Body.GetX1(); ZoneX <= Body.GetX2(); ++ZoneX)
        {
            for (SInt32 ZoneY = Body.GetY1(); ZoneY <= Body.GetY2(); ++ZoneY)
            {
                SearchOnCell(GetCell(Map, ZoneX, ZoneY), Dummy, mResult);
            }
        }

        if (Sorting != Sort::None)
        {
            SortQuery(mResult.GetData(), X, Y, Sorting);
        }
        return mResult.GetData();
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::CreateEntity(Entity & Instigator)
    {
        const Rectangle Body = GetCoordinates(Instigator.GetBody());

        FOR_EACH_ZONE(CreateOnCell, Body, Instigator);

        if (Instigator.GetHeadX() || Instigator.GetHeadY())
        {
            const Rectangle Head = GetCoordinates(Rectangle(Instigator.GetBody(), Instigator.GetHeadX(), Instigator.GetHeadY()));

            FOR_EACH_ZONE(CreateViewerOnCell, Head, Instigator);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::DeleteEntity(Entity & Instigator)
    {
        const Rectangle Body = GetCoordinates(Instigator.GetBody());

        FOR_EACH_ZONE(DeleteOnCell, Body, Instigator);

        if (Instigator.GetHeadX() || Instigator.GetHeadY())
        {
            const Rectangle Head = GetCoordinates(Rectangle(Instigator.GetBody(), Instigator.GetHeadX(), Instigator.GetHeadY()));

            FOR_EACH_ZONE(DeleteViewerOnCell, Head, Instigator);

            mObservations.erase(Instigator.GetID());
        }

        mEntities.erase(Instigator.GetID());
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::UpdateEntity(Entity & Instigator, const Rectangle & Destination, Bool Warped)
    {
        const Rectangle Source  = Instigator.GetBody();

        const Rectangle OldBody = GetCoordinates(Source);
        const Rectangle NewBody = GetCoordinates(Destination);

        Instigator.SetBody(Destination);

        Group Filter;

        for (SInt32 ZoneX = NewBody.GetX1(); ZoneX <= NewBody.GetX2(); ++ZoneX)
        {
            for (SInt32 ZoneY = NewBody.GetY1(); ZoneY <= NewBody.GetY2(); ++ZoneY)
            {
                if (OldBody.Within(ZoneX, ZoneY))
                {
                    UpdateOnCell(GetCell(Instigator.GetMap(), ZoneX, ZoneY), Instigator, Warped, Filter);
                }
                else
                {
                    UpdateOrCreateOnCell(GetCell(Instigator.GetMap(), ZoneX, ZoneY), Instigator, Warped, Filter);
                }
            }
        }

        if (Source != Destination)
        {
            for (SInt32 ZoneX = OldBody.GetX1(); ZoneX <= OldBody.GetX2(); ++ZoneX)
            {
                for (SInt32 ZoneY = OldBody.GetY1(); ZoneY <= OldBody.GetY2(); ++ZoneY)
                {
                    if (!NewBody.Within(ZoneX, ZoneY))
                    {
                        RemoveOnCell(GetCell(Instigator.GetMap(), ZoneX, ZoneY), Instigator);
                    }
                }
            }
        }

        if (Instigator.GetHeadX() || Instigator.GetHeadY())
        {
            const Rectangle OldHead = GetCoordinates(Rectangle(Source, Instigator.GetHeadX(), Instigator.GetHeadY()));
            const Rectangle NewHead = GetCoordinates(Rectangle(Destination, Instigator.GetHeadX(), Instigator.GetHeadY()));

            for (UInt32 Observation : Query(Instigator.GetID(), Sort::None))
            {
                if (mEntities.find(Observation) == mEntities.end()) // WORKAROUND
                {
                    mObservations.at(Instigator.GetID()).Remove(Observation);
                }
                else
                {
                    RemoveOnObserver(mEntities.at(Observation), Instigator);
                }
            }

            for (SInt32 ZoneX = NewHead.GetX1(); ZoneX <= NewHead.GetX2(); ++ZoneX)
            {
                for (SInt32 ZoneY = NewHead.GetY1(); ZoneY <= NewHead.GetY2(); ++ZoneY)
                {
                    if (OldHead.Within(ZoneX, ZoneY))
                    {
                        UpdateViewerOnCell(GetCell(Instigator.GetMap(), ZoneX, ZoneY), Instigator);
                    }
                    else
                    {
                        CreateViewerOnCell(GetCell(Instigator.GetMap(), ZoneX, ZoneY), Instigator);
                    }
                }
            }

            if (OldHead != NewHead)
            {
                for (SInt32 ZoneX = OldHead.GetX1(); ZoneX <= OldHead.GetX2(); ++ZoneX)
                {
                    for (SInt32 ZoneY = OldHead.GetY1(); ZoneY <= OldHead.GetY2(); ++ZoneY)
                    {
                        if (!NewHead.Within(ZoneX, ZoneY))
                        {
                            DeleteViewerOnCell(GetCell(Instigator.GetMap(), ZoneX, ZoneY), Instigator);
                        }
                    }
                }
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::CreateOnCell(Cell & Cell, Entity & Instigator)
    {
        for (Entity * Observer : Cell.Observers)
        {
            CreateOnObserver(Instigator, * Observer);
        }
        Cell.Bodies.push_back(& Instigator);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::DeleteOnCell(Cell & Cell, Entity & Instigator)
    {
        for (Entity * Observer : Cell.Observers)
        {
            DeleteOnObserver(Instigator, * Observer);
        }
        Cell.Bodies.erase_first(& Instigator);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::UpdateOnCell(Cell & Cell, Entity & Instigator, Bool Warped, Group & Filter)
    {
        for (Entity * Observer : Cell.Observers)
        {
            if (Filter.Add(Observer->GetID()))
            {
                UpdateOnObserver(Instigator, * Observer, Warped);
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::UpdateOrCreateOnCell(Cell & Cell, Entity & Instigator, Bool Warped, Group & Filter)
    {
        for (Entity * Observer : Cell.Observers)
        {
            if (Filter.Add(Observer->GetID()))
            {
                UpdateOnObserver(Instigator, * Observer, Warped);
            }
        }
        Cell.Bodies.push_back(& Instigator);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::RemoveOnCell(Cell & Cell, Entity & Instigator)
    {
        for (Entity * Observer : Cell.Observers)
        {
            RemoveOnObserver(Instigator, * Observer);
        }
        Cell.Bodies.erase_first(& Instigator);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::SearchOnCell(Cell & Cell, Entity & Instigator, Group & Filter)
    {
        for (Entity * Observer : Cell.Observers)
        {
            if (SearchOnObserver(Instigator, * Observer))
            {
                Filter.Add(Observer->GetID());
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::CreateViewerOnCell(Cell & Cell, Entity & Instigator)
    {
        for (const Entity * Observer : Cell.Bodies)
        {
            CreateOnObserver(* Observer, Instigator);
        }
        Cell.Observers.push_back(& Instigator);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::DeleteViewerOnCell(Cell & Cell, Entity & Instigator)
    {
        Cell.Observers.erase_first(& Instigator);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::UpdateViewerOnCell(Cell & Cell, Entity & Instigator)
    {
        for (const Entity * Observer : Cell.Bodies)
        {
            CreateOnObserver(* Observer, Instigator);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::CreateOnObserver(const Entity & Instigator, Entity & Observer)
    {
        if (Instigator.GetID() != Observer.GetID())
        {
            const Rectangle Viewport(Observer.GetBody(), Observer.GetHeadX(), Observer.GetHeadY());

            if (Viewport.Intersect(Instigator.GetBody()))
            {
                if (mObservations[Observer.GetID()].Add(Instigator.GetID()))
                {
                    mOnCreate(Instigator.GetID(), Observer.GetID());
                }
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::DeleteOnObserver(const Entity & Instigator, Entity & Observer)
    {
        if (Instigator.GetID() != Observer.GetID())
        {
            const Rectangle Viewport(Observer.GetBody(), Observer.GetHeadX(), Observer.GetHeadY());

            if (Viewport.Intersect(Instigator.GetBody()))
            {
                if (mObservations[Observer.GetID()].Remove(Instigator.GetID()))
                {
                    mOnDelete(Instigator.GetID(), Observer.GetID());
                }
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::UpdateOnObserver(const Entity & Instigator, Entity & Observer, Bool Warped)
    {
        if (Instigator.GetID() != Observer.GetID())
        {
            const Rectangle Viewport(Observer.GetBody(), Observer.GetHeadX(), Observer.GetHeadY());

            if (!Viewport.Intersect(Instigator.GetBody()))
            {
                if (mObservations[Observer.GetID()].Remove(Instigator.GetID()))
                {
                    mOnDelete(Instigator.GetID(), Observer.GetID());
                }
            }
            else
            {
                if (mObservations[Observer.GetID()].Add(Instigator.GetID()))
                {
                    mOnCreate(Instigator.GetID(), Observer.GetID());
                }
                else
                {
                    mOnUpdate(Instigator.GetID(), Observer.GetID(), Warped);
                }
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::RemoveOnObserver(const Entity & Instigator, Entity & Observer)
    {
        if (Instigator.GetID() != Observer.GetID())
        {
            const Rectangle Viewport(Observer.GetBody(), Observer.GetHeadX(), Observer.GetHeadY());

            if (!Viewport.Intersect(Instigator.GetBody()))
            {
                if (mObservations[Observer.GetID()].Remove(Instigator.GetID()))
                {
                    mOnDelete(Instigator.GetID(), Observer.GetID());
                }
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Bool Grid::SearchOnObserver(const Entity & Instigator, Entity & Observer)
    {
        if (Instigator.GetID() != Observer.GetID())
        {
            const Rectangle Viewport(Observer.GetBody(), Observer.GetHeadX(), Observer.GetHeadY());

            return Viewport.Intersect(Instigator.GetBody());
        }
        return false;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Grid::SortQuery(Vector<UInt32> & List, SInt32 X, SInt32 Y, Sort Sorting)
    {
        constexpr auto CalculatePosition = [](SInt32 X1, SInt32 Y1, SInt32 X2, SInt32 Y2) {
            double dx = X2 - X1;
            double dy = Y2 - Y1;
            return sqrt(dx * dx + dy * dy);
        };

        switch (Sorting)
        {
        case Sort::Closest:
        {
            std::sort(List.begin(), List.end(), [&](UInt32 & ID1, UInt32 & ID2)
            {
                Aurora::Collision::Entity * First  = GetEntity(ID1);
                Aurora::Collision::Entity * Second = GetEntity(ID2);

                if (First == nullptr)
                    return true;
                if (Second == nullptr)
                    return false;

                const Real64 Diff1
                    = CalculatePosition(X, Y, First->GetBody().GetCenterX(),  First->GetBody().GetCenterY());
                const Real64 Diff2
                    = CalculatePosition(X, Y, Second->GetBody().GetCenterX(), Second->GetBody().GetCenterY());
                return (Diff1 < Diff2);
            });
            break;
        }
        case Sort::Farthest:
        {
            std::sort(List.begin(), List.end(), [&](UInt32 & ID1, UInt32 & ID2)
            {
                Aurora::Collision::Entity * First  = GetEntity(ID1);
                Aurora:s:Collision::Entity * Second = GetEntity(ID2);

                if (First == nullptr)
                    return false;
                if (Second == nullptr)
                    return true;

                const Real64 Diff1
                    = CalculatePosition(X, Y, First->GetBody().GetCenterX(),  First->GetBody().GetCenterY());
                const Real64 Diff2
                    = CalculatePosition(X, Y, Second->GetBody().GetCenterX(), Second->GetBody().GetCenterY());
                return (Diff1 > Diff2);
            });
            break;
        }
        case Sort::None:
            break;
        }
    }
}