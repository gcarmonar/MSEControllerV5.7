
#include "MSECConfig.h"

/* Global variables in the MSE Controller system available to all components */

/* dummy initialization for N=10 */

float SYS_a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
/*
 * The set of coefficients: a[0] = a1, a[1] = a2, ... a[N-1] = aN
 * array size = N
 * */

float SYS_b[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
/*
 * The set of coefficients: b[0] = b0, b[1] = b1, ... b[N] = bN
 * array size = N + 1
 * */

float SYS_u[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
/*
 * The set of input signal: samples u[0] = u_k, u[1] = u_k-1, ... u[N] = u_k-N
 * array size = N + 1
 * */

float SYS_y[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
/*
 * The set of output signal: y[0] = y_k, y[1] = y_k-1, ... y[N] = y_k-N
 * array size = N + 1
 * */

MSEC_enControlDomain SYS_ControlDomain = MSEC_enZ;
/*
 * Indicates the domain on which the controller shall operate
 */

MSEC_enInputSensor SYS_ControlInputSource = MSEC_enInput1;
/*
 * Indicates which of the control inputs are considered.
 */

unsigned int SYS_SamplingPeriod = 100;
/*
 * Activation time for the cyclic tasks. Task activation period shall be 5X the frequency of the plant.
 * TODO: what are the units handled by the OS_SLEEP?
 */

float SYS_SetPoint = 0;
/*
 * Desired position of the selected car during setup.
 */

float SYS_PosInput1 = 0;
/*
 * Current position of input sensor 1
 */

float SYS_PosInput2 = 0;
/*
 * Current position of input sensor 2
 */

MSEC_enState SYS_MSECState = MSEC_enInvalid;
/*
 * Describes the state of the system.
 * */
