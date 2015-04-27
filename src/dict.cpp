//
// Lol Engine
//
// Copyright: (c) 2010-2013 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://www.wtfpl.net/ for more details.
//

#include <lol/engine-internal.h>

#include <cstring>
#include <cstdlib>

#if defined _WIN32 || defined _XBOX
#   define strcasecmp _stricmp
#endif

namespace lol
{

/*
 * Dict implementation class
 */

class DictData
{
    friend class Dict;

public:
    DictData() :
        nentities(0)
    {
        /* Nothing to do */
    }

    ~DictData()
    {
#if !LOL_BUILD_RELEASE
        if (nentities)
            Log::Error("still %i entities in dict\n", nentities);
#endif
    }

private:
    array<Entity *> m_entities;
    int nentities;
};

/*
 * Public Dict class
 */

Dict::Dict()
  : data(new DictData())
{
}

Dict::~Dict()
{
    delete data;
}

int Dict::MakeSlot(char const *name)
{
    int slotid, empty = -1;

    /* If the entry is already registered, remember its ID. Look for an
     * empty slot at the same time. */
    for (slotid = 0; slotid < data->m_entities.count(); slotid++)
    {
        Entity *e = data->m_entities[slotid];
        if (!e)
        {
            empty = slotid;
            break;
        }
        else
        {
            char const *oldname = e->GetName();
            if (*oldname == '<')
            {
                while (*oldname && *oldname != '>')
                    oldname++;
                while (*oldname == '>')
                    oldname++;
                while (*oldname == ' ')
                    oldname++;
            }

            if (!strcasecmp(name, oldname))
                break;
        }
    }

    /* If this is a new entry, create a new slot for it. */
    if (slotid == data->m_entities.count() || !data->m_entities[slotid])
    {
        if (slotid == data->m_entities.count())
        {
            empty = (int)data->m_entities.count();
            data->m_entities.push(nullptr);
        }

        data->m_entities[empty] = nullptr;
        slotid = empty;
        data->nentities++;
    }
    else
    {
        Ticker::Ref(data->m_entities[slotid]);
    }

    return slotid;
}

void Dict::RemoveSlot(int slotid)
{
    if (Ticker::Unref(data->m_entities[slotid]) == 0)
    {
        data->m_entities[slotid] = nullptr;
        data->nentities--;
    }
}

void Dict::RemoveSlot(Entity *entity)
{
    for (int slotid = 0; slotid < data->m_entities.count(); slotid++)
        if (data->m_entities[slotid] == entity)
        {
            RemoveSlot(slotid);
            return;
        }

#if !LOL_BUILD_RELEASE
    Log::Error("removing unregistered entity %p (%s)\n",
               entity, entity->GetName());
#endif
}

void Dict::SetEntity(int slotid, Entity *entity)
{
    Ticker::Ref(entity);
    data->m_entities[slotid] = entity;
}

Entity *Dict::GetEntity(int slotid)
{
    return data->m_entities[slotid];
}

} /* namespace lol */

