#ifndef PPE_ERROR_H
#define PPE_ERROR_H 1

#include "ppe/types.h"
#include "ppe/macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

enum
{
    PPE_ERR_SUCCEED = 0,
    PPE_ERR_INVALID_ARGUMENT = 1001,
    PPE_ERR_TRY_AGAIN = 1002,
    PPE_ERR_NO_SUCH_ENTRY = 1003,
    PPE_ERR_BAD_UTF8_SEQUENCE = 1004,
    PPE_ERR_OVERFLOW_UPPER_BOUND = 1005,
    PPE_ERR_OVERFLOW_LOWER_BOUND = 1006,
    PPE_ERR_OUT_OF_MEMORY = 1007,
    PPE_ERR_OUT_OF_BUFFER = 1008,
    PPE_ERR_OUT_OF_CAPACITY = 1009,
    PPE_ERR_OUT_OF_RANGE = 1010
};

PPE_API extern void ppe_err_set(ppe_uint code, const ppe_char * restrict msg);

PPE_API extern ppe_uint ppe_err_get_code(void);
PPE_API extern const ppe_char * ppe_err_get_message(void);

#ifdef __cplusplus
}
#endif

#endif /* PPE_ERROR_H */

