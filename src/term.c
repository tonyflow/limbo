// vim:filetype=c:textwidth=80:shiftwidth=4:softtabstop=4:expandtab
#include "term.h"

MAP_IMPL(varmap, var_t, stdname_t, stdname_compar);

int var_compar(var_t l, var_t r)
{
    return l - r;
}

VECTOR_IMPL(varvec, var_t, NULL);

SET_IMPL(varset, var_t, var_compar);

int stdname_compar(stdname_t l, stdname_t r)
{
    return l - r;
}

VECTOR_IMPL(stdvec, stdname_t, NULL);

stdvec_t stdvec_prepend_copy(stdname_t n, const stdvec_t *src)
{
    stdvec_t dst = stdvec_init_with_size(stdvec_size(src) + 1);
    stdvec_append(&dst, n);
    stdvec_append_all(&dst, src);
    return dst;
}

stdvec_t stdvec_copy_append(const stdvec_t *src, stdname_t n)
{
    stdvec_t dst = stdvec_init_with_size(stdvec_size(src) + 1);
    stdvec_append_all(&dst, src);
    stdvec_append(&dst, n);
    return dst;
}

stdvec_t stdvec_copy_append_all(const stdvec_t *src, const stdvec_t *elems)
{
    stdvec_t dst = stdvec_init_with_size(stdvec_size(src) + 1);
    stdvec_append_all(&dst, src);
    stdvec_append_all(&dst, elems);
    return dst;
}

SET_IMPL(stdvecset, stdvec_t *, stdvec_cmp);

SET_IMPL(stdset, stdname_t, NULL);

