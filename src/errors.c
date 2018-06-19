#include <assert.h>

#include "ppe/errors.h"

#ifdef __cplusplus
extern "C"
{
#endif

enum
{
    PPE_ERR_BASIC_START = 1001,
    PPE_ERR_BASIC_END = 1011
};

static ppe_char * g_basic_messages[] =
{
    "Invalid argument is passed", /* PPE_ERR_INVALID_ARGUMENT = 1001 */
    "Operation failed or would block, try again", /* PPE_ERR_TRY_AGAIN = 1002 */
    "No such entry to the given key or name", /* PPE_ERR_NO_SUCH_ENTRY = 1003 */
    "The string contains bad UTF-8 sequences", /* PPE_ERR_BAD_UTF8_SEQUENCE = 1004 */
    "The value is overflow the upper bound", /* PPE_ERR_OVERFLOW_UPPER_BOUND = 1005 */
    "The value is overflow the lower bound", /* PPE_ERR_OVERFLOW_LOWER_BOUND = 1006 */
    "Out of memory", /* PPE_ERR_OUT_OF_MEMORY = 1007 */
    "Out of buffer", /* PPE_ERR_OUT_OF_BUFFER = 1008 */
    "Out of buffer", /* PPE_ERR_OUT_OF_CAPACITY = 1009 */
    "Out of range" /* PPE_ERR_OUT_OF_RANGE = 1010 */
};

static ppe_uint g_code;
static ppe_char * g_message;

PPE_API void ppe_err_set(ppe_uint code, const ppe_char * restrict msg)
{
    g_code = code;
    if (msg) {
        g_message = msg;
    } else if (code != 0) {
        assert(PPE_ERR_BASIC_START <= code && code < PPE_ERR_BASIC_END);
        g_message = g_basic_messages[code - PPE_ERR_BASIC_START];
    } else {
        g_message = "Operation succeed";
    }
}

PPE_API ppe_uint ppe_err_get_code(void)
{
    return g_code;
}

PPE_API const ppe_char * ppe_err_get_message(void)
{
    return g_message;
}

#ifdef __cplusplus
}
#endif
