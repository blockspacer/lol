//
// Deus Hax (working title)
// Copyright (c) 2010 Sam Hocevar <sam@hocevar.net>
//

//
// The Entity class
// ---------------
// Entities are objects that can be ticked by the game loop and/or the render
// loop. Entities are implemented as one or several linked lists. See the
// Ticker class for the ticking logic and the linked list implementation.
//

#if !defined __DH_ENTITY_H__
#define __DH_ENTITY_H__

#include <stdint.h>

class Entity
{
    friend class Ticker;
    friend class TickerData;
    friend class Dict;

protected:
    typedef enum
    {
        GROUP_BEFORE = 0,
        GROUP_DEFAULT,
        GROUP_AFTER,
        GROUP_DRAW_CAPTURE,
        // Must be the last element
        GROUP_COUNT
    }
    Group;

    Entity();
    virtual ~Entity();

    virtual char const *GetName();
    virtual Group GetGroup();

    virtual void TickGame(float deltams);
    virtual void TickDraw(float deltams);

    Entity *next, *autonext;
    int ref, autorelease, destroy;

#if !FINAL_RELEASE
    enum
    {
        STATE_IDLE = 0,
        STATE_PRETICK_GAME,
        STATE_POSTTICK_GAME,
        STATE_PRETICK_DRAW,
        STATE_POSTTICK_DRAW,
    }
    state;
#endif
};

#endif // __DH_ENTITY_H__

