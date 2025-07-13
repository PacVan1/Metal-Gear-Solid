#pragma once

class Actor;

struct HitResult
{
    bool    hit;
    vec3    point;
    vec3    normal;
    Actor*  actor;
};



namespace RayCaster
{
    void        Init();
    HitResult   Cast(vec3 const& from, vec3 const& to, btDynamicsWorld const& world, s32 const group = 0, s32 const mask = 0, bool const drawLine = false);
    HitResult   Cast(vec3 const& from, vec3 const& to, s32 const group = 0, s32 const mask = 0, bool const drawLine = false);
    void        Render();
}   

namespace RC = RayCaster;