//
//  Lol Engine
//
//  Copyright © 2010—2015 Sam Hocevar <sam@hocevar.net>
//            © 2013—2015 Benjamin “Touky” Huet <huet.benjamin@gmail.com>
//
//  Lol Engine is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#pragma once

#include <lol/base/array.h>
#include <lol/debug/lines.h>
#include <lol/image/color.h>

namespace lol
{

//------ AABB TREE SYSTEM -----
template <typename TE, typename TV, typename TB, size_t child_nb> class AABBTree;
template <typename TE> class Quadtree;
template <typename TE> class Octree;

//--
namespace Debug {
//--

template<typename TREE, typename TE, typename TBB>
void DrawInner(TREE *tree, array<TBB, vec4> &boxes,
               array<TE *, int, vec4> &elements,
               array<int, TBB> &leaves, int children, vec4 color)
{
    boxes.push(tree->GetAABB(), Color::white);
    leaves.push(0, boxes.last().m1);
    while (leaves.count() > 0)
    {
        for (int j = 0; j < tree->GetTree()[leaves[0].m1].m_elements.count(); j++)
        {
            bool done = false;
            for (int k = 0; k < elements.count(); k++)
            {
                if (elements[k].m1 == tree->GetElements()[tree->GetTree()[leaves[0].m1].m_elements[j]].m_element)
                {
                    elements[k].m2++;
                    done = true;
                    break;
                }
            }
            if (!done)
                elements.push(tree->GetElements()[tree->GetTree()[leaves[0].m1].m_elements[j]].m_element, 1, Color::red);
        }

        for (int i = 0; i < children; i++)
        {
            if (tree->GetTree()[leaves[0].m1].m_children[i] != 0)
            {
                TBB bbox = tree->GetSubAABB(leaves[0].m2, i);
                leaves.push(tree->GetTree()[leaves[0].m1].m_children[i], bbox);
                boxes.push(bbox, color);
            }
        }
        leaves.remove(0);
    }
}

//--
template <typename TE, typename TV = void>
void Draw(Quadtree<TE>* tree, vec4 color)
{
    array<box2, vec4> boxes;
    array<TE*, int, vec4> elements;
    array<int, box2> leaves;

    DrawInner<Quadtree<TE>, TE, box2>(tree, boxes, elements, leaves, 4, color);

    vec3 off = vec3(0.0f, 0.1f, 0.0f);
    //vec3 add = vec3(0.0f, 0.1f, 0.0f);
    while (boxes.count() > 0)
    {
        Debug::DrawBox(vec3(boxes[0].m1.aa.x, tree->m_debug_y_offset, boxes[0].m1.aa.y),
                       vec3(boxes[0].m1.bb.x, tree->m_debug_y_offset, boxes[0].m1.bb.y),
                       boxes[0].m2);
        boxes.remove(0);
    }
    while (elements.count() > 0)
    {
        while (elements[0].m2 > 0)
        {
            Debug::DrawBox(vec3(elements[0].m1->GetAABB().aa.x, tree->m_debug_y_offset, elements[0].m1->GetAABB().aa.y) + off * (float)elements[0].m2,
                           vec3(elements[0].m1->GetAABB().bb.x, tree->m_debug_y_offset, elements[0].m1->GetAABB().bb.y) + off * (float)elements[0].m2,
                           elements[0].m3);
            elements[0].m2--;
        }
        elements.remove(0);
    }
}
//--
template <typename TE, typename TV = void>
void Draw(Octree<TE>* tree, vec4 color)
{
    array<box3, vec4> boxes;
    array<TE*, int, vec4> elements;
    array<int, box3> leaves;

    DrawInner<Octree<TE>, TE, box3>(tree, boxes, elements, leaves, 8, color);

    vec3 off = vec3(0.0f, 0.1f, 0.0f);
    //vec3 add = vec3(0.0f, 0.1f, 0.0f);
    while (boxes.count() > 0)
    {
        //float size = boxes[0].m1.bb.x - boxes[0].m1.aa.x;
        Debug::DrawBox(vec3(boxes[0].m1.aa.x, boxes[0].m1.aa.y, boxes[0].m1.aa.z) /* + off * (m_size.x / size) */,
                        vec3(boxes[0].m1.bb.x, boxes[0].m1.bb.y, boxes[0].m1.bb.z) /* + off * (m_size.x / size) */,
                        boxes[0].m2);
        //off += add;
        boxes.remove(0);
    }
    while (elements.count() > 0)
    {
        while (elements[0].m2 > 0)
        {
            Debug::DrawBox(vec3(elements[0].m1->GetAABB().aa.x, elements[0].m1->GetAABB().aa.y, elements[0].m1->GetAABB().aa.z) + off * (float)elements[0].m2,
                            vec3(elements[0].m1->GetAABB().bb.x, elements[0].m1->GetAABB().bb.y, elements[0].m1->GetAABB().bb.z) + off * (float)elements[0].m2,
                            elements[0].m3);
            elements[0].m2--;
        }
        elements.remove(0);
    }
}
//--
}

//--
template <typename TE, typename TV, typename TB, size_t child_nb>
class AABBTree
{
    struct NodeLeaf
    {
        int             m_parent;
        //Children pos in the list
        int             m_children[child_nb];
        //Element list
        array<int>      m_elements;

        NodeLeaf(int parent)
        {
            m_parent = parent;
            for (size_t i = 0; i < child_nb; ++i)
                m_children[i] = 0;
        }
    };

    struct TreeElement
    {
        TE*             m_element;
        array<int>      m_leaves;

        inline bool operator==(const TE*& element) { return m_element == element; }
    };

public:
    AABBTree()
    {
        m_max_depth = 1;
        m_max_element = 1;
        AddLeaf(0);
    }
    ~AABBTree()
    {
        Clear();
    }
    void CopySetup(const AABBTree<TE, TV, TB, child_nb>* src)
    {
        CopySetup(*src);
    }
    void CopySetup(const AABBTree<TE, TV, TB, child_nb>& src)
    {
        m_size = src.m_size;
        m_max_depth = src.m_max_depth;
        m_max_element = src.m_max_element;
    }

private:
    //--
    bool CleanupEmptyLeaves(int leaf=0)
    {
        int empty_children = 0;
        for (size_t i = 0; i < child_nb; ++i)
        {
            bool child_empty = false;
            if (m_tree[leaf].m_children[i] != 0)
                child_empty = CleanupEmptyLeaves(m_tree[leaf].m_children[i]);
            empty_children += (int)(m_tree[leaf].m_children[i] == 0 || child_empty);
        }
        if (empty_children == 4 && leaf != 0)
        {
            for (size_t i = 0; i < child_nb; ++i)
            {
                int old_leaf = m_tree[leaf].m_children[i];
                if (old_leaf != 0)
                    m_free_leaves << old_leaf;
                m_tree[leaf].m_children[i] = 0;
            }
            return true;
        }
        return false;
    }
    //--
    void RemoveElement(TE* element)
    {
        int idx = INDEX_NONE;
        for (int i = 0; i < m_elements.count(); ++i)
            if (m_elements[i].m_element == element)
                idx = i;

        if (idx == INDEX_NONE)
            return;

        //Remove item from tree leaves
        for (int i = 0; i < m_elements[idx].m_leaves.count(); i++)
            m_tree[m_elements[idx].m_leaves[i]].m_elements.removeItem(idx);

        //Try leaves cleanup
        CleanupEmptyLeaves();
    }
    //--
    int AddElement(TE* element)
    {
        for (int i = 0; i < m_elements.count(); ++i)
            if (m_elements[i].m_element == element)
                return i;

        TreeElement new_element;
        new_element.m_element = element;
        new_element.m_leaves = array<int>();
        m_elements << new_element;
        return m_elements.count() - 1;
    }
    //--
    int AddLeaf(int parent)
    {
        int idx = m_tree.count();
        if (m_free_leaves.count())
        {
            idx = m_free_leaves.pop();
            m_tree[idx] = NodeLeaf(parent);
        }
        else
            m_tree << NodeLeaf(parent);
        return idx;
    }

    //--
    bool TestLeaf(int leaf, const TB& leaf_bb, const TB& test_bb, array<TE*>& elements)
    {
        bool result = false;
        if (TestAABBVsAABB(leaf_bb, test_bb))
        {
            NodeLeaf& node = m_tree[leaf];
            for (size_t i = 0; i < child_nb; ++i)
            {
                if (node.m_children[i] != 0)
                {
                    TB sub_aabb = GetSubAABB(leaf_bb, (int)i);
                    result = result | TestLeaf(node.m_children[i], sub_aabb, test_bb, elements);
                }
                else
                {
                    for (int j = 0; j < node.m_elements.count(); j++)
                        elements.push_unique(m_elements[node.m_elements[j]].m_element);
                    result = true;
                }
            }
        }
        return result;
    }
    //--
    bool RegisterElement(TE* element, int leaf, TB leaf_bb, int depth)
    {
        if (TestAABBVsAABB(leaf_bb, element->GetAABB()))
        {
            bool found_child = false;
            for (size_t i = 0; i < child_nb; ++i)
            {
                TB child_bb = GetSubAABB(leaf_bb, (int)i);
                int child_leaf = m_tree[leaf].m_children[i];
                //there is a child, go further down
                if (child_leaf != 0)
                    found_child = found_child | RegisterElement(element, child_leaf, child_bb, depth + 1);
            }
            if (found_child)
                return true;

            //Too much elements, we need to re-dispatch the elements
            if (m_tree[leaf].m_elements.count() + 1 > m_max_element &&
                depth < m_max_depth)
            {
                //Extract elements
                array<int> elements = m_tree[leaf].m_elements;
                elements.push_unique(AddElement(element));
                m_tree[leaf].m_elements.clear();
                //Add children
                for (size_t j = 0; j < child_nb; ++j)
                    m_tree[leaf].m_children[j] = AddLeaf(leaf);
                //Re-run extracted elements
                while (elements.count())
                {
                    RegisterElement(m_elements[elements[0]].m_element, leaf, leaf_bb, depth);
                    elements.remove(0);
                }
            }
            //else add to list.
            else
            {
                int idx = AddElement(element);
                m_elements[idx].m_leaves.push_unique(leaf);
                m_tree[leaf].m_elements.push_unique(idx);
            }
            return true;
        }
        return false;
    }

public:
    void                RegisterElement(TE* element)                        { RegisterElement(element, 0, GetAABB(), 0); }
    void                UnregisterElement(TE* element)                      { RemoveElement(element); }
    bool                FindElements(const TB& bbox, array<TE*>& elements)  { return TestLeaf(0, GetAABB(), bbox, elements); }
    void                Clear()
    {
        m_tree.clear();
        m_elements.clear();
    }

    //--
    virtual TV          GetSubOffset(int sub) = 0;
    virtual TB          GetAABB() { return TB(-m_size * .5f, m_size * .5f); }
    virtual TB          GetSubAABB(const TB& bbox, int sub)
    {
        TV v(GetSubOffset(sub));
        TV half_vec = bbox.extent() * .5f;
        return TB(bbox.aa + half_vec * v,
                  bbox.aa + half_vec * (v + TV(1.f)));
    }

    //--
    TV                  GetSize()                       { return m_size; }
    int                 GetMaxDepth()                   { return m_max_depth; }
    int                 GetMaxElement()                 { return m_max_element; }
    void                SetSize(TV size)                { m_size = size; }
    void                SetMaxDepth(int max_depth)      { m_max_depth = max_depth; }
    void                SetMaxElement(int max_element)  { m_max_element = max_element; }

    array<NodeLeaf> const & GetTree() const
    {
        return m_tree;
    }

    array<TreeElement> const & GetElements() const
    {
        return m_elements;
    }

protected:
    array<NodeLeaf>     m_tree;         //actual tree
    array<TreeElement>  m_elements;     //elements to leaves
    array<int>          m_free_leaves;  //leaves removed from tree
    TV                  m_size;         //Main tree size
    int                 m_max_depth;    //Maximum depth possible
    int                 m_max_element;  //Maximum element per leaf
};

//--
template <typename TE>
class Quadtree : public AABBTree<TE, vec2, box2, 4>
{
    friend void Debug::Draw<TE,void>(Octree<TE>* tree, vec4 color);
public:
    Quadtree()          { m_debug_y_offset = 0.f; }
    virtual ~Quadtree() { }
    float               m_debug_y_offset;
protected:
    virtual vec2        GetSubOffset(int sub) { return vec2(ivec2(sub % 2, sub / 2)); }
};

//--
template <typename TE>
class Octree : public AABBTree<TE, vec3, box3, 8>
{
    friend void Debug::Draw<TE,void>(Octree<TE>* tree, vec4 color);
public:
    Octree()            { }
    virtual ~Octree()   { }
protected:
    virtual vec3        GetSubOffset(int sub) { return vec3(ivec3(sub % 2, sub / 4, (sub % 4) / 2)); }
};

} /* namespace lol */

