#include "common.h"
#include "ray_caster.h"

namespace RayCaster
{
    struct DebugLine
    {
        vec3 from;
        vec3 to;
        vec3 color;
    };

    u32 constexpr			MAX_DEBUG_LINES = 10;
    std::deque<DebugLine>	gDebugLines;
    GLuint                  gDBGVAO;
    GLuint                  gDBGVBO;

    struct DebugLineVertex
    {
        vec3 point;
        vec3 color;
    };

    void Init()
    {
        gDBGVBO = OGL::VBO();
        size_t const size = sizeof(DebugLineVertex) * 2 * MAX_DEBUG_LINES;
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

        gDBGVAO = OGL::VAO();

        glBindBuffer(GL_ARRAY_BUFFER, gDBGVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugLineVertex), (void*)offsetof(DebugLineVertex, point));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DebugLineVertex), (void*)offsetof(DebugLineVertex, color));
        glEnableVertexAttribArray(0); // position
        glEnableVertexAttribArray(1); // color
    }

    static void AddDebugLine(DebugLine const& line)
    {
#ifdef DEBUG_MODE
        if (gDebugLines.size() >= MAX_DEBUG_LINES) 
            gDebugLines.pop_front();

        gDebugLines.push_back(line);
#endif
    }

    void Render()
    {
        static GLuint const PROGRAM = Shader::LoadAndBind("debug_line", { BINDING_CAM });

        vector<float> vertexData;
        for (const auto& line : gDebugLines) 
        {
            const auto& s = line.from;
            const auto& e = line.to;
            const auto& c = line.color;

            vertexData.insert(vertexData.end(), { s.x, s.y, s.z, c.x, c.y, c.z });
            vertexData.insert(vertexData.end(), { e.x, e.y, e.z, c.x, c.y, c.z });
        }

        glBindBuffer(GL_ARRAY_BUFFER, gDBGVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.size() * sizeof(float), vertexData.data());

        Shader::Bind(PROGRAM);

        glLineWidth(3.0f);
        glBindVertexArray(gDBGVAO);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertexData.size() / 6));
    }

    HitResult Cast(vec3 const& from, vec3 const& to, btDynamicsWorld const& world, s32 const group, s32 const mask, bool const drawLine)
    {
        btCollisionWorld::ClosestRayResultCallback rayCallback(ToBT(from), ToBT(to));

        rayCallback.m_collisionFilterGroup  = group;
        rayCallback.m_collisionFilterMask   = mask;

        world.rayTest(ToBT(from), ToBT(to), rayCallback);

        HitResult result;
        result.hit = rayCallback.hasHit();
        if (result.hit)
        {
            btRigidBody* body   = const_cast<btRigidBody*>(btRigidBody::upcast(rayCallback.m_collisionObject));
            result.actor        = static_cast<Actor*>(body->getUserPointer());
            result.point        = ToGLM(rayCallback.m_hitPointWorld);
            result.normal       = ToGLM(rayCallback.m_hitNormalWorld);
            if (drawLine) AddDebugLine({ from, result.point, { 0.0f, result.hit, 0.0f } });
            return result;
        }
        if (drawLine) AddDebugLine({ from, to, vec3(1.0f) });
        return result;
    }

    HitResult Cast(vec3 const& from, vec3 const& to, s32 const group, s32 const mask, bool const drawLine)
    {
        return Cast(from, to, *SM().GetDynaWorld().mSimulation, group, mask, drawLine);
    }
}