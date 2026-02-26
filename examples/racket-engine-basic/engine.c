#include "engine.h"

int
engine_resume(int id)
{
    return sequencer_capture(id) + 1;
}

int
engine_compute(int id)
{
    int a = engine_resume(id);
    int b = sequencer_capture(id + 10);
    return a + b;
}
