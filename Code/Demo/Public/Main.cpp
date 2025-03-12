#include "Collision/Grid.hpp"

using namespace Aurora::Collision;

void OnCreate(UInt32 Entity, UInt32 Observer)
{
    printf("OnCreate %d %d\n", Entity, Observer);
}
void OnDelete(UInt32 Entity, UInt32 Observer)
{
    printf("OnDelete %d %d\n", Entity, Observer);
}
void OnUpdate(UInt32 Entity, UInt32 Observer, Bool Warped)
{
    printf("OnUpdate %d %d %s\n", Entity, Observer, Warped ? "(T)" : "(M)");
}

int main()
{
    const UInt32 DEFAULT_MAP = 0;

    Grid World(300);

    World.Attach(OnCreate, OnDelete, OnUpdate);
    World.Create(1, 9, 7, 283, 82, 43, 0, 1);
    World.Create(666, 8, 6, 283, 81, 44, 0, 1);
    World.Create(2, 9, 7, 283, 82, 44, 0, 1);
    World.Delete(2);
    World.Create(2, 9, 7, 1, 50, 50, 0, 1);
    World.Update(2, 1, 74, 64, false);


    return 0;
}