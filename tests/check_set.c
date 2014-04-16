// vim:filetype=c:textwidth=80:shiftwidth=4:softtabstop=4:expandtab
#include <check.h>
#include <stdlib.h>
#include "../src/set.h"

static int compar_long_int(long int p, long int q)
{
    return p - q;
}

SET_DECL(iset, long int);
SET_IMPL(iset, long int, compar_long_int);

START_TEST(test_set_add)
{
    iset_t set = iset_init();
    iset_t set_all = iset_init();
    for (int i = 0; i < 10; ++i) {
        iset_add(&set, i);
        iset_add(&set, (i + 10));
        iset_add(&set, i);
    }
    ck_assert_int_eq(iset_size(&set), 20);
    iset_add_all(&set_all, &set);
    for (int i = 0; i < 20; ++i) {
        ck_assert_int_eq(iset_find(&set, i), i);
        ck_assert(iset_contains(&set, i));
    }
    for (int i = 20; i < 30; ++i) {
        ck_assert_int_eq(iset_find(&set, -i), -1);
        ck_assert(!iset_contains(&set, -i));
    }
    for (int i = 5; i < 15; ++i) {
        ck_assert(iset_contains(&set, i));
        iset_remove(&set, i);
        ck_assert(!iset_contains(&set, i));
    }
    ck_assert_int_eq(iset_size(&set), 10);
    iset_add_all(&set_all, &set);
    for (int i = 5; i < 15; ++i) {
        ck_assert(!iset_contains(&set, i));
        iset_add(&set, i);
        ck_assert(iset_contains(&set, i));
    }
    for (int i = 15; i > 5; --i) {
        ck_assert(iset_contains(&set, i));
        iset_remove(&set, i);
        ck_assert(!iset_contains(&set, i));
    }
    ck_assert_int_eq(iset_size(&set), 10);
    iset_add_all(&set_all, &set);
    for (int i = 15; i > 5; --i) {
        ck_assert(!iset_contains(&set, i));
        iset_add(&set, i);
        ck_assert(iset_contains(&set, i));
    }
    ck_assert_int_eq(iset_size(&set), 20);
    iset_add_all(&set_all, &set);
    ck_assert_int_eq(iset_size(&set), 20);
    iset_cleanup(&set_all);
    iset_cleanup(&set);
}
END_TEST

void check_set(const iset_t *set, int n)
{
    ck_assert_int_eq(iset_size(set), n);
    for (int i = 1; i < iset_size(set); ++i) {
        ck_assert_int_lt(iset_get(set, i-1), iset_get(set, i));
    }
}

START_TEST(test_set_replace)
{
    iset_t set = iset_init();
    for (int i = 0; i < 100; ++i) {
        iset_add(&set, i+1);
    }
    for (int i = 0; i < 100; ++i) {
        ck_assert_int_eq(iset_get(&set, i), i+1);
    }
    check_set(&set, 100);
    ck_assert_int_eq(iset_size(&set), 100);
    for (int i = 0; i < 100; ++i) {
        const int j = iset_replace(&set, i+1, i);
        ck_assert_int_ne(j, -1);
        ck_assert_int_eq(iset_size(&set), 100);
        ck_assert_int_eq(j, i);
        ck_assert_int_eq(iset_get(&set, j), i);
        ck_assert_int_lt(i, iset_size(&set));
        if (0 < i) {
            ck_assert_int_lt(iset_get(&set, i-1), iset_get(&set, i));
        }
        if (i < iset_size(&set) - 1) {
            ck_assert_int_lt(iset_get(&set, i), iset_get(&set, i+1));
        }
    }
    check_set(&set, 100);
    /*
    ck_assert_int_eq(iset_size(&set), 100);
    for (int i = 0; i < 100; ++i) {
        const int j = iset_replace(&set, i, 99-i);
        ck_assert_int_eq(j, -1);
    }
    */
    iset_clear(&set);
    for (int i = 0; i < 100; ++i) {
        iset_add(&set, 2*i);
    }
    for (int i = 0; i < 100; ++i) {
        const int j = 199 - 2*i;
        const int k = iset_replace(&set, 2*i, j);
        ck_assert_int_eq(iset_get(&set, k), j);
        ck_assert_int_eq(iset_size(&set), 100);
    }
    check_set(&set, 100);
    iset_clear(&set);
    for (int i = 0; i < 100; ++i) {
        iset_add(&set, 2*i);
    }
    for (int i = 99; i >= 0; --i) {
        const int j = 199 - 2*i;
        const int k = iset_replace(&set, 2*i, j);
        ck_assert_int_eq(iset_get(&set, k), j);
        ck_assert_int_eq(iset_size(&set), 100);
    }
    check_set(&set, 100);
}
END_TEST

START_TEST(test_set_copy)
{
    iset_t src = iset_init();
    iset_t dst;
    ck_assert_int_eq(iset_size(&src), 0);
    dst = iset_copy(&src);
    ck_assert_int_eq(iset_size(&dst), 0);
    iset_cleanup(&dst);
    for (int i = 0; i < 100; ++i) {
        iset_add(&src, i);
    }
    ck_assert_int_eq(iset_size(&src), 100);
    dst = iset_copy(&src);
    ck_assert_int_eq(iset_size(&dst), 100);
    iset_cleanup(&dst);
    iset_cleanup(&src);
}
END_TEST

START_TEST(test_set_lazy_copy)
{
    iset_t src = iset_init();
    iset_t dst;
    ck_assert_int_eq(iset_size(&src), 0);
    dst = iset_lazy_copy(&src);
    ck_assert_int_eq(iset_size(&dst), 0);
    iset_cleanup(&dst);
    for (int i = 0; i < 100; ++i) {
        iset_add(&src, i);
    }
    ck_assert_int_eq(iset_size(&src), 100);
    dst = iset_lazy_copy(&src);
    ck_assert(iset_eq(&src, &dst));
    for (int i = 0; i < 100; ++i) {
        ck_assert_int_eq(iset_get(&dst, i), i);
    }
    for (int i = 50; i >= 0; --i) {
        iset_remove(&dst, 2*i);
    }
    ck_assert(!iset_eq(&src, &dst));
    for (int i = 50; i >= 0; --i) {
        iset_remove(&src, 2*i);
    }
    ck_assert(iset_eq(&src, &dst));
    iset_cleanup(&dst);
    dst = iset_lazy_copy(&src);
    ck_assert(iset_eq(&src, &dst));
    for (int i = 50; i >= 0; --i) {
        iset_add(&dst, 2*i);
    }
    ck_assert(!iset_eq(&src, &dst));
    for (int i = 50; i >= 0; --i) {
        iset_add(&src, 2*i);
    }
    ck_assert(iset_eq(&src, &dst));
    iset_cleanup(&dst);
    iset_cleanup(&src);
}
END_TEST

START_TEST(test_set_singleton)
{
    iset_t set = iset_singleton(5);
    ck_assert_int_eq(iset_size(&set), 1);
    ck_assert(iset_contains(&set, 5));
    ck_assert(!iset_contains(&set, 4));
    ck_assert(!iset_contains(&set, 6));
    iset_clear(&set);
    ck_assert_int_eq(iset_size(&set), 0);
    iset_cleanup(&set);
}
END_TEST

START_TEST(test_set_union)
{
    iset_t set1 = iset_init();
    iset_t set2 = iset_init();
    iset_t set;
    // left and right empty
    ck_assert_int_eq(iset_size(&set1), 0);
    ck_assert_int_eq(iset_size(&set2), 0);
    set = iset_union(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 0);
    ck_assert(iset_eq(&set, &set1));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left empty
    ck_assert_int_eq(iset_size(&set1), 0);
    for (int i = 0; i < 10; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    set = iset_union(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    ck_assert(iset_eq(&set, &set2));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // right empty
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    ck_assert_int_eq(iset_size(&set2), 0);
    set = iset_union(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    ck_assert(iset_eq(&set, &set1));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left same as right
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    for (int i = 0; i < 10; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    set = iset_union(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    ck_assert(iset_eq(&set, &set1));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left subset of right
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    for (int i = 0; i < 20; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 20);
    set = iset_union(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 20);
    ck_assert(iset_eq(&set, &set2));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left superset of right
    for (int i = 0; i < 20; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 20);
    for (int i = 0; i < 10; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    set = iset_union(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 20);
    ck_assert(iset_eq(&set, &set1));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left and right intersect
    for (int i = 0; i < 20; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 20);
    for (int i = 10; i < 30; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 20);
    set = iset_union(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 30);
    for (int i = 0; i < 30; ++i) {
        ck_assert_int_eq(iset_get(&set, i), i);
    }
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left and right intersect twice
    for (int i = 0; i < 20; ++i) {
        iset_add(&set1, i);
        iset_add(&set1, (i + 40));
    }
    ck_assert_int_eq(iset_size(&set1), 40);
    for (int i = 10; i < 30; ++i) {
        iset_add(&set2, i);
        iset_add(&set2, (i + 40));
    }
    ck_assert_int_eq(iset_size(&set2), 40);
    set = iset_union(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 60);
    for (int i = 0; i < 30; ++i) {
        ck_assert_int_eq(iset_get(&set, i), i);
    }
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left and right don't intersect
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    for (int i = 10; i < 20; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    set = iset_union(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 20);
    for (int i = 0; i < 20; ++i) {
        ck_assert_int_eq(iset_get(&set, i), i);
    }
    iset_cleanup(&set);
    iset_cleanup(&set1);
    iset_cleanup(&set2);
}
END_TEST

START_TEST(test_set_difference)
{
    iset_t set1 = iset_init();
    iset_t set2 = iset_init();
    iset_t set;
    // left and right empty
    ck_assert_int_eq(iset_size(&set1), 0);
    ck_assert_int_eq(iset_size(&set2), 0);
    ck_assert(iset_contains_all(&set1, &set2));
    ck_assert(iset_contains_all(&set2, &set1));
    set = iset_difference(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 0);
    ck_assert(iset_eq(&set, &set1));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left empty
    ck_assert_int_eq(iset_size(&set1), 0);
    for (int i = 0; i < 10; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    ck_assert(!iset_contains_all(&set1, &set2));
    ck_assert(iset_contains_all(&set2, &set1));
    set = iset_difference(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 0);
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // right empty
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    ck_assert_int_eq(iset_size(&set2), 0);
    ck_assert(iset_contains_all(&set1, &set2));
    ck_assert(!iset_contains_all(&set2, &set1));
    set = iset_difference(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    ck_assert(iset_eq(&set, &set1));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left same as right
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    for (int i = 0; i < 10; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    ck_assert(iset_contains_all(&set1, &set2));
    ck_assert(iset_contains_all(&set2, &set1));
    set = iset_difference(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 0);
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left subset of right
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    for (int i = 0; i < 20; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 20);
    ck_assert(!iset_contains_all(&set1, &set2));
    ck_assert(iset_contains_all(&set2, &set1));
    set = iset_difference(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 0);
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left superset of right
    for (int i = 0; i < 20; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 20);
    for (int i = 0; i < 10; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    ck_assert(iset_contains_all(&set1, &set2));
    ck_assert(!iset_contains_all(&set2, &set1));
    set = iset_difference(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    for (int i = 0; i < 10; ++i) {
        ck_assert_int_eq(iset_get(&set, i), i + 10);
    }
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left and right intersect
    for (int i = 0; i < 20; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 20);
    for (int i = 10; i < 30; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 20);
    ck_assert(!iset_contains_all(&set1, &set2));
    ck_assert(!iset_contains_all(&set2, &set1));
    set = iset_difference(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    for (int i = 0; i < 10; ++i) {
        ck_assert_int_eq(iset_get(&set, i), i);
    }
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left and right intersect twice
    for (int i = 0; i < 20; ++i) {
        iset_add(&set1, i);
        iset_add(&set1, (i + 40));
    }
    ck_assert_int_eq(iset_size(&set1), 40);
    for (int i = 10; i < 30; ++i) {
        iset_add(&set2, i);
        iset_add(&set2, (i + 40));
    }
    ck_assert_int_eq(iset_size(&set2), 40);
    ck_assert(!iset_contains_all(&set1, &set2));
    ck_assert(!iset_contains_all(&set2, &set1));
    set = iset_difference(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 20);
    for (int i = 0; i < 10; ++i) {
        ck_assert_int_eq(iset_get(&set, i), i);
        ck_assert_int_eq(iset_get(&set, i + 10), i + 40);
    }
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left and right don't intersect
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    for (int i = 10; i < 20; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    ck_assert(!iset_contains_all(&set1, &set2));
    ck_assert(!iset_contains_all(&set2, &set1));
    set = iset_difference(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    ck_assert(iset_eq(&set, &set1));
    iset_cleanup(&set);
    iset_cleanup(&set1);
    iset_cleanup(&set2);
}
END_TEST

START_TEST(test_set_remove_all)
{
    iset_t set1 = iset_init();
    iset_t set2 = iset_init();
    iset_t set;
    // left and right empty
    ck_assert_int_eq(iset_size(&set1), 0);
    ck_assert_int_eq(iset_size(&set2), 0);
    set = iset_copy(&set1);
    iset_remove_all(&set, &set2);
    ck_assert_int_eq(iset_size(&set), 0);
    ck_assert(iset_eq(&set, &set1));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left empty
    ck_assert_int_eq(iset_size(&set1), 0);
    for (int i = 0; i < 10; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    set = iset_copy(&set1);
    iset_remove_all(&set, &set2);
    ck_assert_int_eq(iset_size(&set), 0);
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // right empty
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    ck_assert_int_eq(iset_size(&set2), 0);
    set = iset_copy(&set1);
    iset_remove_all(&set, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    ck_assert(iset_eq(&set, &set1));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left same as right
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    for (int i = 0; i < 10; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    set = iset_copy(&set1);
    iset_remove_all(&set, &set2);
    ck_assert_int_eq(iset_size(&set), 0);
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left subset of right
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    for (int i = 0; i < 20; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 20);
    set = iset_copy(&set1);
    iset_remove_all(&set, &set2);
    ck_assert_int_eq(iset_size(&set), 0);
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left superset of right
    for (int i = 0; i < 20; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 20);
    for (int i = 0; i < 10; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    set = iset_copy(&set1);
    iset_remove_all(&set, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    for (int i = 0; i < 10; ++i) {
        ck_assert_int_eq(iset_get(&set, i), i + 10);
    }
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left and right intersect
    for (int i = 0; i < 20; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 20);
    for (int i = 10; i < 30; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 20);
    set = iset_copy(&set1);
    iset_remove_all(&set, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    for (int i = 0; i < 10; ++i) {
        ck_assert_int_eq(iset_get(&set, i), i);
    }
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left and right intersect twice
    for (int i = 0; i < 20; ++i) {
        iset_add(&set1, i);
        iset_add(&set1, (i + 40));
    }
    ck_assert_int_eq(iset_size(&set1), 40);
    for (int i = 10; i < 30; ++i) {
        iset_add(&set2, i);
        iset_add(&set2, (i + 40));
    }
    ck_assert_int_eq(iset_size(&set2), 40);
    set = iset_copy(&set1);
    iset_remove_all(&set, &set2);
    ck_assert_int_eq(iset_size(&set), 20);
    for (int i = 0; i < 10; ++i) {
        ck_assert_int_eq(iset_get(&set, i), i);
        ck_assert_int_eq(iset_get(&set, i + 10), i + 40);
    }
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left and right don't intersect
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    for (int i = 10; i < 20; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    set = iset_copy(&set1);
    iset_remove_all(&set, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    ck_assert(iset_eq(&set, &set1));
    iset_cleanup(&set);
    iset_cleanup(&set1);
    iset_cleanup(&set2);
}
END_TEST

START_TEST(test_set_intersection)
{
    iset_t set1 = iset_init();
    iset_t set2 = iset_init();
    iset_t set;
    // left and right empty
    ck_assert_int_eq(iset_size(&set1), 0);
    ck_assert_int_eq(iset_size(&set2), 0);
    set = iset_intersection(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 0);
    ck_assert(iset_eq(&set, &set1));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left empty
    ck_assert_int_eq(iset_size(&set1), 0);
    for (int i = 0; i < 10; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    set = iset_intersection(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 0);
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // right empty
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    ck_assert_int_eq(iset_size(&set2), 0);
    set = iset_intersection(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 0);
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left same as right
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    for (int i = 0; i < 10; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    set = iset_intersection(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    ck_assert(iset_eq(&set, &set1));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left subset of right
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    for (int i = 0; i < 20; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 20);
    set = iset_intersection(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    ck_assert(iset_eq(&set, &set1));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left superset of right
    for (int i = 0; i < 20; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 20);
    for (int i = 0; i < 10; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    set = iset_intersection(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    ck_assert(iset_eq(&set, &set2));
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left and right intersect
    for (int i = 0; i < 20; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 20);
    for (int i = 10; i < 30; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 20);
    set = iset_intersection(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 10);
    for (int i = 0; i < 10; ++i) {
        ck_assert_int_eq(iset_get(&set, i), i + 10);
    }
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left and right intersect twice
    for (int i = 0; i < 20; ++i) {
        iset_add(&set1, i);
        iset_add(&set1, (i + 40));
    }
    ck_assert_int_eq(iset_size(&set1), 40);
    for (int i = 10; i < 30; ++i) {
        iset_add(&set2, i);
        iset_add(&set2, (i + 40));
    }
    ck_assert_int_eq(iset_size(&set2), 40);
    set = iset_intersection(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 20);
    for (int i = 0; i < 10; ++i) {
        ck_assert_int_eq(iset_get(&set, i), i + 10);
        ck_assert_int_eq(iset_get(&set, i + 10), i + 50);
    }
    iset_cleanup(&set);
    iset_clear(&set1);
    iset_clear(&set2);
    // left and right don't intersect
    for (int i = 0; i < 10; ++i) {
        iset_add(&set1, i);
    }
    ck_assert_int_eq(iset_size(&set1), 10);
    for (int i = 10; i < 20; ++i) {
        iset_add(&set2, i);
    }
    ck_assert_int_eq(iset_size(&set2), 10);
    set = iset_intersection(&set1, &set2);
    ck_assert_int_eq(iset_size(&set), 0);
    iset_cleanup(&set);
    iset_cleanup(&set1);
    iset_cleanup(&set2);
}
END_TEST

Suite *iset_suite(void)
{
  Suite *s = suite_create("Set");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_set_add);
  tcase_add_test(tc_core, test_set_replace);
  tcase_add_test(tc_core, test_set_copy);
  tcase_add_test(tc_core, test_set_lazy_copy);
  tcase_add_test(tc_core, test_set_singleton);
  tcase_add_test(tc_core, test_set_union);
  tcase_add_test(tc_core, test_set_difference);
  tcase_add_test(tc_core, test_set_remove_all);
  tcase_add_test(tc_core, test_set_intersection);
  suite_add_tcase(s, tc_core);
  return s;
}

int main(void)
{
    int number_failed;
    Suite *s = iset_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

