#pragma once

#include "DescriptorHandle.h"

namespace SamplerManager {

    extern DescriptorHandle LinearWrap;
    extern DescriptorHandle LinearClamp;
    extern DescriptorHandle LinearBorder;

    void Initialize();

}