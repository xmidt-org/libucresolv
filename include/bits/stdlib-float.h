/* No floating-point inline functions in rtld and for the conform tests.  */
#define STDLIB_FLOAT_IS_IN(lib) (IN_MODULE == MODULE_##lib)

#ifdef _ISOMAC
# include <glibc-stdlib/bits/stdlib-float.h>
#else
#if !STDLIB_FLOAT_IS_IN (rtld)
#  include <glibc-stdlib/bits/stdlib-float.h>
#endif
#endif
