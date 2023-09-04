#pragma once

#define SOKOL_IMPL
/* this is only needed for the debug-inspection headers */
#define SOKOL_TRACE_HOOKS

// Do not sort includes
// clang-format off

#include "sokol_app.h"
#include "sokol_args.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"
#include "sokol_time.h"

#include "util/sokol_shape.h"

#include "imgui/imgui.h"
#include "util/sokol_imgui.h"
#include "util/sokol_gfx_imgui.h"

// clang-format on