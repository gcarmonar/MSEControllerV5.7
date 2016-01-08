#ifndef SYSCONFIG_H
#define SYSCONFIG_H

typedef enum
{
	MSEC_enZ = 1,
	MSEC_enS = 2
} MSEC_enControlDomain;

typedef enum
{
	MSEC_enNoInput = 0,
	MSEC_enInput1 = 1,		/* eQEP input 1 */
	MSEC_enInput2 = 2		/* eQEP input 2 */
} MSEC_enInputSensor;

typedef enum
{
	MSEC_enInvalid = 0,
	MSEC_enSetup,			/* The system is rceiving the Control parameters from the host */
	MSEC_enRunning,			/* The controller is in execution, y() is updated according u(), and y() is streamed to the host */
	MSEC_enReporting		/* (In case it's not possible to stream y() to the host) Transmit the log of y() to the host */
} MSEC_enState;

#define SYS_N	11
/*
 * The order of the Z/S domain transfer function
 * */
#if SYS_N <= 1
	#error Attention N (SYS_N) shall be higher than 1!
#endif

extern float SYS_a[];
/*
 * The set of coefficients: a[0] = a1, a[1] = a2, ... a[N-1] = aN
 * */

extern float SYS_b[];
/*
 * The set of coefficients: b[0] = b0, b[1] = b1, ... b[N] = bN
 * */

extern float SYS_u[];
/*
 * The set of input signal: samples u[0] = u_k, u[1] = u_k-1, ... u[N] = u_k-N
 * */

extern float SYS_y[];
/*
 * The set of output signal: y[0] = y_k, y[1] = y_k-1, ... y[N] = y_k-N
 * */

extern MSEC_enControlDomain SYS_ControlDomain;
/*
 * Indicates the domain on which the controller shall operate
 */

extern MSEC_enInputSensor SYS_ControlInputSource;
/*
 * Indicates which of the control inputs are considered.
 */

extern unsigned int SYS_SamplingPeriod;
/*
 * Activation time for the cyclic tasks. Task activation period shall be 5X the frequency of the plant.
 */

extern float SYS_SetPoint;
/*
 * Desired position of the selected car during setup.
 */

extern float SYS_PosInput1;
/*
 * Current position of input sensor 1
 */

extern float SYS_PosInput2;
/*
 * Current position of input sensor 2
 */

extern MSEC_enState SYS_MSECState;
/*
 * Describes the state of the system.
 * */

#endif
