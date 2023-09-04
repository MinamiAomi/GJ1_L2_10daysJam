#pragma once

#include "Graphics/ColorBuffer.h"

class RenderManager {
public:
    static RenderManager* GetInstance();


private:

    ColorBuffer sceneColorBuffer;


};