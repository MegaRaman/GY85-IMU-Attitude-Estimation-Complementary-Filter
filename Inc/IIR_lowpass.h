#ifndef INC_IIR_LOWPASS_H_
#define INC_IIR_LOWPASS_H_

/*
 * struct IIR_lowpass - struct that manages EMA IIR lowpass
 *
 * @alpha - main parameter that determines cutoff frequency. Filtered data is calculated that way: y[n] = alpha * current_measurement + (1 - alpha) * y[n - 1]
 * @out - array of three filtered measurements of x, y and z axis
 * */
struct IIR_lowpass {
    float alpha;
    float out[3];
};

/*
 * init_lowpass - Initialize struct IIR_lowpass
 * */
void init_lowpass(struct IIR_lowpass *filter, float alpha);

/*
 * update_lowpass - Update filter with new measurements.
 * New output will be stored in IIR_lowpass.out array
 * */
void update_lowpass(struct IIR_lowpass *filter, int sample_x, int sample_y, int sample_z);

#endif /* INC_IIR_LOWPASS_H_ */
