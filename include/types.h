#ifndef TYPES_S
#define TYPES_S

typedef unsigned int uint;
typedef unsigned long long ulint;

/* Status */
typedef enum
{
  e_failure,
  e_success
} Status;

/* Status with error msg */
typedef struct
{
  Status status;
  char *msg;
} StatusInfo;

/* opearion type */
typedef enum
{
  e_unsupported,
  e_disp,
  e_mod,
  e_extract,
  e_mImage
} Operation;

#endif
