#include "engine.h"
#include "trace.h"

int
engine_step(int id)
{
    trace_save(id);
    return trace_load(id) + 1;
}
