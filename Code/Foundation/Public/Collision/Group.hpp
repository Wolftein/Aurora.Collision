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

#include "Core/Core.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Aurora::Collision
{
    // -=(Undocumented)=-
    class Group
    {
    public:

        // -=(Undocumented)=-
        Vector<UInt32> & GetData()
        {
            return mCollection;
        }

        void Copy(Group & Other)
        {
            mCollection = Other.mCollection;
        }

        // -=(Undocumented)=-
        bool Add(UInt32 ID)
        {
            auto Iterator = eastl::lower_bound(mCollection.begin(), mCollection.end(), ID);

            if (Iterator == mCollection.end() || * Iterator != ID)
            {
                mCollection.insert(Iterator, ID);
                return true;
            }
            return false;
        }

        // -=(Undocumented)=-
        bool Remove(UInt32 ID)
        {
            const auto Lower = eastl::lower_bound(mCollection.begin(), mCollection.end(), ID);

            if (Lower != std::end(mCollection) && * Lower == ID)
            {
                const auto Upper = std::upper_bound(Lower, std::end(mCollection), ID);
                mCollection.erase(Lower, Upper);
                return true;
            }
            return false;
        }

        // -=(Undocumented)=-
        bool Contains(UInt32 ID)
        {
            const auto Iterator = std::lower_bound(mCollection.begin(), mCollection.end(), ID);
            return Iterator != mCollection.end() && * Iterator == ID;
        }

        // -=(Undocumented)=-
        void Clear()
        {
            mCollection.clear();
        }

    public:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Vector<UInt32> mCollection;
    };
}