#pragma once

#include <web/web_system.h>
#include <xanim/dobj.h>
#include <xanim/xanim.h>

#include <cmath>
#include <cstdint>

// Log only the first requested invalid bone. The canonical calculation and
// its existing assertions still decide how execution proceeds.
inline void Web_LogInvalidDObjMats(const DObj_s *obj, const char *stage, const int *partBits)
{
    if (!obj || !obj->skel.mat || !partBits)
        return;

    for (int bone = 0; bone < obj->numBones; ++bone)
    {
        if (!(static_cast<std::uint32_t>(partBits[bone >> 5])
            & (0x80000000u >> (bone & 31))))
            continue;

        const DObjAnimMat &mat = obj->skel.mat[bone];
        if (std::isfinite(mat.quat[0]) && std::isfinite(mat.quat[1])
            && std::isfinite(mat.quat[2]) && std::isfinite(mat.quat[3])
            && std::isfinite(mat.trans[0]) && std::isfinite(mat.trans[1])
            && std::isfinite(mat.trans[2]) && std::isfinite(mat.transWeight))
            continue;

        const char *boneName = DObjGetBoneName(obj, bone);
        Web_Log(WebLogLevel::Error,
            "Invalid DObj matrix at %s: entity=%u model=%s bone=%d name=%s quat=(%g,%g,%g,%g) trans=(%g,%g,%g) weight=%g\n",
            stage, obj->entnum, DObjGetName(obj), bone,
            boneName ? boneName : "unknown",
            mat.quat[0], mat.quat[1], mat.quat[2], mat.quat[3],
            mat.trans[0], mat.trans[1], mat.trans[2], mat.transWeight);
        if (obj->tree && obj->tree->anims)
        {
            char animations[2048]{};
            DObjDisplayAnimToBuffer(obj, "Active animations:\n", animations, sizeof(animations));
            Web_Log(WebLogLevel::Error, "%s", animations);
        }
        break;
    }
}
