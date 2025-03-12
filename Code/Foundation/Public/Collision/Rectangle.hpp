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

#include "Core/Core.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Aurora::Collision
{
    // -=(Undocumented)=-
    class Rectangle
    {
    public:

        // -=(Undocumented)=-
        Rectangle(SInt32 X1, SInt32 Y1, SInt32 X2, SInt32 Y2)
            : mX1 { X1 },
              mY1 { Y1 },
              mX2 { X2 },
              mY2 { Y2 }
        {
        }

        // -=(Undocumented)=-
        Rectangle(const Rectangle & Other, SInt32 RadiusX, SInt32 RadiusY)
             : Rectangle(Other.mX1 - RadiusX,
                         Other.mY1 - RadiusY,
                         Other.mX2 + RadiusX,
                         Other.mY2 + RadiusY)
        {
        }

        // -=(Undocumented)=-
        SInt32 GetX1() const
        {
            return mX1;
        }

        // -=(Undocumented)=-
        SInt32 GetY1() const
        {
            return mY1;
        }

        // -=(Undocumented)=-
        SInt32 GetX2() const
        {
            return mX2;
        }

        // -=(Undocumented)=-
        SInt32 GetY2() const
        {
            return mY2;
        }

        // -=(Undocumented)=-
        SInt32 GetWidth() const
        {
            return (mX2 - mX1);
        }

        // -=(Undocumented)=-
        SInt32 GetHeight() const
        {
            return (mX2 - mX1);
        }

        // -=(Undocumented)=-
        SInt32 GetCenterX() const
        {
            return mX1 + GetWidth() / 2;
        }

        // -=(Undocumented)=-
        SInt32 GetCenterY() const
        {
            return mY1 + GetHeight() / 2;
        }

        // -=(Undocumented)=-
        Bool Intersect(const Rectangle & Other) const
        {
            return (mX1 <= Other.mX2 && Other.mX1 <= mX2 && mY1 <= Other.mY2 && Other.mY1 <= mY2);
        }

        // -=(Undocumented)=-
        Bool Within(SInt32 X, SInt32 Y) const
        {
            return X >= mX1 && Y >= mY1 && X <= mX2 && Y <= mY2;
        }

        // -=(Undocumented)=-
        Bool operator==(const Rectangle & Other) const
        {
            return (mX1 == Other.mX1 && mY1 == Other.mY1 && mX2 == Other.mX2 && mY2 == Other.mY2);
        }

        // -=(Undocumented)=-
        Bool operator!=(const Rectangle & Other) const
        {
            return (mX1 != Other.mX1 || mY1 != Other.mY1 || mX2 != Other.mX2 || mY2 != Other.mY2);
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        SInt32 mX1;
        SInt32 mY1;
        SInt32 mX2;
        SInt32 mY2;
    };
}