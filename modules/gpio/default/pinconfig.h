#ifndef _PINCONFIG_H__
#define _PINCONFIG_H__
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct GPIO_ADDRESS{
 int gpio;
 int bias_address;
}GPIONODE;

GPIONODE gpionumber[]={ };


#ifdef __cplusplus
}
#endif // __cplusplus
//-----------------------------------------------------------------------------

#endif //
