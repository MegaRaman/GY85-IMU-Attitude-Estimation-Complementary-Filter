#include "IIR_lowpass.h"

void init_lowpass(struct IIR_lowpass *filter, float alpha) {
    if (alpha < 0.0f) {
        filter->alpha = 0.0f;
    }
    else if (alpha > 1.0f) {
        filter->alpha = 1.0f;
    }
    else {
        filter->alpha = alpha;
    }

    memset(filter->out, 0, sizeof(float) * 3);
}

void update_lowpass(struct IIR_lowpass *filter, int sample_x, int sample_y, int sample_z) {
    filter->out[0] = filter->alpha * sample_x + (1 - filter->alpha) * filter->out[0];
    filter->out[1] = filter->alpha * sample_y + (1 - filter->alpha) * filter->out[1];
    filter->out[2] = filter->alpha * sample_z + (1 - filter->alpha) * filter->out[2];
}
