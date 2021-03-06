//
//  Lol Engine
//
//  Copyright © 2009—2013 Cédric Lecacheur <jordx@free.fr>
//            © 2009—2013 Benjamin “Touky” Huet <huet.benjamin@gmail.com>
//            © 2010—2018 Sam Hocevar <sam@hocevar.net>
//
//  Lol Engine is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#pragma once

#include "commandstack.h"
#include "easymeshrender.h"
#include "easymeshbuild.h"

#include <map>

//
// EasyMesh: A class about generating 3D mesh without using the hands
//           Mesh can be generated using C++ or lua code
//

namespace lol
{

//CSGUsage --------------------------------------------------------------------
/* A safe enum for MeshCSG operations. */
struct CSGUsageBase : public StructSafeEnum
{
    enum Type
    {
        Union,
        Substract,
        SubstractLoss, // will remove B from A, but not add inverted B
        And,
        Xor
    };
protected:
    virtual bool BuildEnumMap(std::map<int64_t, std::string>& enum_map)
    {
        enum_map[Union] = "Union";
        enum_map[Substract] = "Substract";
        enum_map[SubstractLoss] = "SubstractLoss";
        enum_map[And] = "And";
        enum_map[Xor] = "Xor";
        return true;
    }
};
typedef SafeEnum<CSGUsageBase> CSGUsage;

//MeshTransform ---------------------------------------------------------------
struct MeshTransformBase : public StructSafeEnum
{
    enum Type
    {
        Taper,
        Twist,
        Bend,
        Stretch,
        Shear
    };
protected:
    virtual bool BuildEnumMap(std::map<int64_t, std::string>& enum_map)
    {
        enum_map[Taper] = "Taper";
        enum_map[Twist] = "Twist";
        enum_map[Bend] = "Bend";
        enum_map[Stretch] = "Stretch";
        enum_map[Shear] = "Shear";
        return true;
    }
};
typedef SafeEnum<MeshTransformBase> MeshTransform;

class EasyMesh : public Mesh
{
    friend class EasyMeshParser;
    friend class GpuEasyMeshData;

public:
    EasyMesh();
    EasyMesh(const EasyMesh& em);
    MeshRender GetMeshState() { return m_state; }
    bool SetRender(bool should_render);

    //-------------------------------------------------------------------------
    //Render-build operation
    //-------------------------------------------------------------------------
    void MeshConvert();

    //-------------------------------------------------------------------------
    //Command-build (lua now) operations
    //-------------------------------------------------------------------------
    bool Compile(char const *command, bool Execute = true);
    void ExecuteCmdStack(bool ExecAllStack = true);

private:
    void UpdateVertexDict(array< int, int > &vertex_dict);

    //-------------------------------------------------------------------------
    //Mesh CSG operations
    //-------------------------------------------------------------------------
public:
    /* [cmd:csgu] Performs a Union operation as (mesh0_Outside + mesh1_Outside) */
    void CsgUnion();
    /* [cmd:csgs] Performs a Substract operation as (mesh0_Outside + mesh1_Inside-inverted) */
    void CsgSub();
    /* [cmd:csgsl] Performs a Substract operation without keeping the mesh1 part */
    void CsgSubL();
    /* [cmd:csga] Performs an And operation as (mesh0_Inside + mesh1_Inside) */
    void CsgAnd();
    /* [cmd:csgx] Performs a Xor operation as (m0_Outside/m0_Inside-inverted + m1_Outside/m1_Inside-inverted) */
    void CsgXor();

private:
    void MeshCsg(CSGUsage csg_operation);

    //-------------------------------------------------------------------------
    //Mesh Cursor operations
    //-------------------------------------------------------------------------
public:
    /* [cmd:lp[ ]] will perform a loop of loopnb */
    void LoopStart(int loopnb);
    /* No cmd, implicit ] */
    void LoopEnd();
    /* [cmd:[] from this point onward, any operation will not be performed on previous vertices */
    void OpenBrace();
    /* [cmd:]] Merge current vertices with previous context */
    void CloseBrace();
    /* [cmd:tsw] When active, on negative-scaling, normal-vector correction will not occur */
    void ToggleScaleWinding();
    /* [cmd:tqw] When active, quad will have a fifth center vertex */
    void ToggleQuadWeighting();
    /* [cmd:tpbn] When active, normal will be computed after the build */
    void TogglePostBuildNormal();
    /* [cmd:tpbn] When active, prevents vertices cleanup */
    void ToggleVerticeNoCleanup();
    /* [cmd:sc] Set both color */
    void SetCurColor(vec4 const &color);
    /* [cmd:sca] Set base color A */
    void SetCurColorA(vec4 const &color);
    /* [cmd:scb] Set base color B */
    void SetCurColorB(vec4 const &color);
    /* [cmd:scv] Sets all vertices in this scope color. */
    void SetVertColor(vec4 const &color);

    //-------------------------------------------------------------------------
    //Internal : Basic triangle/vertex operations
    //-------------------------------------------------------------------------
private:
    void AddVertex(vec3 const &coord);
    void AddDupVertex(int i);
    void AddLerpVertex(int i, int j, float alpha);
    void AddLerpVertex(VertexData const &vi, VertexData const &vj, float alpha);
    VertexData GetLerpVertex(int i, int j, float alpha);
    VertexData GetLerpVertex(VertexData const &vi, VertexData const &vj, float alpha);
    void AddQuad(int i1, int i2, int i3, int i4, int base, bool duplicate = false);
    void AddTriangle(int i1, int i2, int i3, int base, bool duplicate = false);
    void ComputeNormals(int start, int vcount);
public:
    /* Remove all unused */
    void VerticesCleanup();
    /* Merge vertices AKA: smooth */
    void VerticesMerge();
    /* Merge vertices AKA: Flat */
    void VerticesSeparate();
public: //DEBUG
    void ComputeTexCoord(float uv_scale, int uv_offset);

    //-------------------------------------------------------------------------
    //Internal : Vertices operations
    //-------------------------------------------------------------------------
private:
    void SetTexCoordData(vec2 const &new_offset, vec2 const &new_scale);
    void SetTexCoordData2(vec2 const &new_offset, vec2 const &new_scale);

    void SetCurVertNormal(vec3 const &normal);
    void SetCurVertColor(vec4 const &color);
    void SetCurVertTexCoord(vec2 const &texcoord);
    void SetCurVertTexCoord2(vec2 const &texcoord);

public:
    //-------------------------------------------------------------------------
    //Mesh transform operations
    //-------------------------------------------------------------------------
    /* See Translate */
    void TranslateX(float t);
    /* See Translate */
    void TranslateY(float t);
    /* See Translate */
    void TranslateZ(float t);
    /* [cmd:t/tx/ty/tz] Translate vertices
        - v : Translation quantity.
     */
    void Translate(vec3 const &v);
    /* See Rotate */
    void RotateX(float degrees);
    /* See Rotate */
    void RotateY(float degrees);
    /* See Rotate */
    void RotateZ(float degrees);
    /* [cmd:r/rx/ry/rz] Rotate vertices
        - degrees : rotation quantity.
        - axis : rotation axis.
     */
    void Rotate(float degrees, vec3 const &axis);
    /* [cmd:rj] Randomly move vertices along Origin-to-vertex as f(vtx) = vtx + o2v * (1.0 + rand(r))
        - r : jitter maximum value.
     */
    void RadialJitter(float r);
    /* [cmd:tax] multiply axis y&z by x as f(y) = y * (1.0 + (ny * x + xoff))
        - ny : value of n for y.
        - nz : value of n for z.
        - xoff : value of xoff.
        - absolute (def:true) : if (true) Multiply will use an absolute x.
     */
    void TaperX(float ny, float nz, float xoff=0.f, bool absolute=true);
    /* [cmd:tay] Same as TaperX, with Y */
    void TaperY(float nx, float nz, float yoff=0.f, bool absolute=true);
    /* [cmd:taz] Same as TaperX, with Z */
    void TaperZ(float nx, float ny, float zoff=0.f, bool absolute=true);
    /* [cmd:twx] Twist vertices around x axis with x as rotation value as f(p) = (RotateX(x * t + toff) * p)
        - t : Angle multiplier.
        - toff : Applied offset.
     */
    void TwistX(float t, float toff=0.f);
    /* [cmd:twy] Same as TwistX, with Y */
    void TwistY(float t, float toff=0.f);
    /* [cmd:twz] Same as TwistX, with Z */
    void TwistZ(float t, float toff=0.f);
    /* [cmd:shx] Shear vertices using x value as shear quantity as f(y) = y + (ny * x + xoff)
        - ny : Value of n for y.
        - nz : Value of n for z.
        - xoff : Value of xoff.
        - absolute (def:true) : if (true) Multiply will use an absolute x.
     */
    void ShearX(float ny, float nz, float xoff=0.f, bool absolute=true);
    /* [cmd:shy] Same as ShearX, with Y */
    void ShearY(float nx, float nz, float yoff=0.f, bool absolute=true);
    /* [cmd:shz] Same as ShearX, with Z */
    void ShearZ(float nx, float ny, float zoff=0.f, bool absolute=true);
    /* [cmd:stx] Stretch vertices using x value as stretch quantity as f(y) = y + (pow(x, ny) + xoff)
        - ny : Value of n for y.
        - nz : Value of n for z.
        - xoff : Value of xoff.
     */
    void StretchX(float ny, float nz, float xoff=0.f);
    /* [cmd:sty] Same as StretchX, with Y */
    void StretchY(float nx, float nz, float yoff=0.f);
    /* [cmd:stz] Same as StretchX, with Z */
    void StretchZ(float nx, float ny, float zoff=0.f);
    /* [cmd:bdxy] Bend vertices using x as bend quantity along y axis using f(p) = (RotateY(x * t + toff) * p)
        - t : Angle multiplier.
        - xoff : Applied offset.
     */
    void BendXY(float t, float toff=0.f);
    /* [cmd:bdxz] Same as BendXY, with X & Z */
    void BendXZ(float t, float toff=0.f);
    /* [cmd:bdyx] Same as BendXY, with Y & X */
    void BendYX(float t, float toff=0.f);
    /* [cmd:bdyz] Same as BendXY, with Y & Z */
    void BendYZ(float t, float toff=0.f);
    /* [cmd:bdzx] Same as BendXY, with Z & X */
    void BendZX(float t, float toff=0.f);
    /* [cmd:bdzy] Same as BendXY, with Z & Y */
    void BendZY(float t, float toff=0.f);
private:
    void DoMeshTransform(MeshTransform ct, Axis axis0, Axis axis1, float n0, float n1, float noff, bool absolute=false);
public:
    /* [cmd:s/sx/sy/sz] Scale vertices
        - s : scale quantity.
     */
    void Scale(float s);
    void ScaleX(float s);
    void ScaleY(float s);
    void ScaleZ(float s);
    void Scale(vec3 const &s);
    /* [cmd:m*] Mirror vertices through *-plane
        Acts as an OpenBrace
     */
    void MirrorX();
    void MirrorY();
    void MirrorZ();
    /* [no-cmd] Duplicates vertices and scale duplicate
        Acts as an OpenBrace
     */
    void DupAndScale(vec3 const &s, bool open_brace=false);
    /* [cmd:ch] Performs a chamfer operation //TODO : Make it work
        - f : Chamfer quantity.
     */
    void Chamfer(float f);
    /* [cmd:splt] split triangles in 4 smaller ones
        - pass : Number of pass applied.
     */
    void SplitTriangles(int pass);
private:
    void SplitTriangles(int pass, VertexDictionnary *vert_dict);
public:
    /* [cmd:smth] Smooth the mesh by subdivising it.
        - pass : a pass is made of (n0 split then n1 smooth) repeat.
        - split_per_pass : n0 value in above explanation.
        - smooth_per_pass : n1 value in above explanation.
     */
    void SmoothMesh(int pass, int split_per_pass, int smooth_per_pass);

    //-------------------------------------------------------------------------
    //Mesh shape primitive operations
    //-------------------------------------------------------------------------
    /* [cmd:ac] Cylinder centered on (0,0,0) with BBox [-.5*max(d1, d2), -.5*h, -.5*max(d1, d2)]
        - nbsides : Number of sides.                   [+.5*max(d1, d2), +.5*h, +.5*max(d1, d2)]
        - h : Height of the cylinder.
        - d1 : Lower diameter.
        - d2 : Upper diameter.
        - dualside : if (true) will also create inner sides : TOOD:TOREMOVE?? : needed ?
        - smooth : if (true) will smooth normals : TOOD:TOREMOVE : smooth should be handled elsewhere
        - close : if (true) will add discs to close the cylinder
     */
    void AppendCylinder(int nsides, float h, float d1, float d2,
                        bool dualside=false, bool smooth=false, bool close=false);
    /* [cmd:asph] Sphere centered on (0,0,0) with BBox [-.5*d][.5*d]
        - ndivisions : number of subdivisions each Sphere triangle will sustain.
        - d : Diameter.
     */
    void AppendSphere(int ndivisions, float d);
    /* [cmd:acap] Capsule centered on (0,0,0) with BBox [-.5*d, -(.5*d+h), -.5*d][.5*d, (.5*d+h), .5*d]
        - ndivisions : number of subdivisions each Sphere triangle will sustain.
        - h : Inner height.
        - d : Diameter.
     */
    void AppendCapsule(int ndivisions, float h, float d);
    /* [cmd:ato] Torus centered on (0,0,0) with BBox [-.5*d2][.5*d2]
        - ndivisions : number of subdivisions of the torus.
        - d1 : Inner diameter.
        - d2 : Outer diameter.
     */
    void AppendTorus(int ndivisions, float d1, float d2);
    /* [cmd:ab] Box centered on (0,0,0) with BBox [-.5 * size][.5 * size]
        - size : size of the box.
        - chamf : size of the chamfer.
     */
    void AppendBox(vec3 const &size, float chamf=0.f);
    //Same as AppendBox
    void AppendSmoothChamfBox(vec3 const &size, float chamf);
    //Same as AppendBox
    void AppendFlatChamfBox(vec3 const &size, float chamf);
    //Same as AppendBox
    void AppendBox(vec3 const &size, float chamf, bool smooth);
    /* [cmd:as]
       Append a Star centered on (0,0,0) contained within a disc of "max(d1, d2)" diameter.
        - nbranches : Number of branches.
        - d1 : double Length of the branches.
        - d2 : double Length of the "branch" located between d1-branches.
        - fade : if (true) in-between branches use ColorB.
        - fade2 : if (true) Star branches use ColorB.
     */
    void AppendStar(int nbranches, float d1, float d2,
                    bool fade=false, bool fade2=false);
    /* [cmd:aes] Star centered on (0,0,0) contained within a disc of "max(max(d1, d2), max(d1 + extrad, d2 + extrad))" diameter.
       Expanded star branches use ColorB.
        - nbranches : Number of branches.
        - d1 : Double Length of the branches.
        - d2 : Double Length of the "branch" located between r1-branches.
        - extrad : Extra length added to expand all branches.
     */
    void AppendExpandedStar(int nbranches, float d1, float d2, float extrad=0.f);
    /* [cmd:ad] Disc centered on (0,0,0) with d diameter.
        - nbsides : Number of sides.
        - d : Diameter.
        - fade : if (true) Outer vertices will use ColorB
     */
    void AppendDisc(int nsides, float d, bool fade=false);
    /* [cmd:at] Triangle centered on (0,0,0) contained within a disc of "d" diameter.
        - d : diameter of the containing disc..
        - fade : if (true) 2nd & 3rd Vertices will use ColorB
     */
    void AppendSimpleTriangle(float d, bool fade=false);
    /* [cmd:aq] Quad centered on (0,0,0) contained within BBox [-size*.5f, 0, -size*.5f][size*.5f, 0, size*.5f]
        - size : Size of quad.
        - fade : if (true) 3rd & 4th Vertices will use ColorB
     */
    void AppendSimpleQuad(float size, bool fade=false);
private:
    //complex version of above one
    void AppendSimpleQuad(vec2 p1, vec2 p2, float z=0.f, bool fade=false);
public:
    /* [cmd:acg] Gear centered on (0,0,0) contained within BBox [-.5*max(d1,d2), -.5*h, -.5*max(d1, d2)]
        - h : Height of the Gear.                               [+.5*max(d1,d2), +.5*h, +.5*max(d1, d2)]
        - d10 : Upper Inner diameter.
        - d20 : Lower Inner diameter.
        - d1  : Upper Outer diameter.
        - d2  : Lower Outer diameter.
        - d12 : Upper Cog diameter.
        - d22 : Lower Cog diameter.
        - sidemul : multiplier for the size of the cogs.
        - offset : useless
     */
    void AppendCog(int nbsides, float h, float d10, float d20, float d11,
                   float d21, float d12, float d22, float sidemul = 0.f, bool offset = false);
    void AppendCog(int nbsides, float h, vec2 d0, vec2 d1, vec2 d2, float sidemul = 0.f, bool offset = false)
    {
        AppendCog(nbsides, h, d0.x, d0.y, d1.x, d1.y, d2.x, d2.y, sidemul, offset);
    }

    //-------------------------------------------------------------------------
    //TODO : Mesh Bone operations
    //-------------------------------------------------------------------------
    //void AddBone(int parent_id) {}

    //Convenience functions
public:
    int GetVertexCount() { return m_vert.count(); }
    vec3 const &GetVertexLocation(int i) { return m_vert[i].m_coord; }

//private:
    array<uint16_t>     m_indices;
    array<VertexData>   m_vert;

    //<vert count, indices count>
    array<int, int>     m_cursors;

    MeshRender          m_state;

public:
    inline EasyMeshBuildData* BD()
    {
        if (!m_build_data)
            m_build_data = new EasyMeshBuildData();
        return m_build_data;
    };
private:
    class EasyMeshBuildData* m_build_data;
};

} /* namespace lol */

