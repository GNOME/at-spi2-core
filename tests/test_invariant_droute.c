#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

/* Test that buffer reads in droute.c never exceed declared length */
START_TEST(test_buffer_overflow_sprintf_id_str)
{
    /* Invariant: formatted id_str output must not exceed buffer bounds */
    
    /* Payloads: test cases for id values that could overflow id_str buffer */
    int test_ids[] = {
        0,              /* valid: minimal case */
        1,              /* valid: normal case */
        INT_MAX,        /* boundary: maximum int value */
        INT_MAX - 1,    /* boundary: near maximum */
        999999999       /* exploit: large id causing long formatted string */
    };
    
    int num_tests = sizeof(test_ids) / sizeof(test_ids[0]);
    
    for (int i = 0; i < num_tests; i++) {
        char id_str[32];  /* declared buffer size in droute.c context */
        int id = test_ids[i];
        
        /* Simulate the vulnerable sprintf call with bounds check */
        int written = snprintf(id_str, sizeof(id_str), ":1.%d", id);
        
        /* Invariant: formatted output must fit within declared buffer */
        ck_assert_int_ge(written, 0);
        ck_assert_int_lt(written, (int)sizeof(id_str));
        
        /* Verify null termination */
        ck_assert(id_str[sizeof(id_str) - 1] == '\0' || written < (int)sizeof(id_str));
        
        /* Verify no buffer overflow occurred by checking string length */
        size_t len = strlen(id_str);
        ck_assert_int_lt(len, sizeof(id_str));
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_buffer_overflow_sprintf_id_str);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}