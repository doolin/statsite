#include <check.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include "cm_quantile.h"

START_TEST(test_cm_init_and_destroy)
{
    cm_quantile cm;
    double quants[] = {0.5, 0.90, 0.99};
    int res = init_cm_quantile(0.01, (double*)&quants, 3, &cm);
    fail_unless(res == 0);
    res = destroy_cm_quantile(&cm);
    fail_unless(res == 0);
}
END_TEST

START_TEST(test_cm_init_no_quants)
{
    cm_quantile cm;
    int res = init_cm_quantile(0.01, NULL, 0, &cm);
    fail_unless(res == -1);
}
END_TEST

START_TEST(test_cm_init_bad_quants)
{
    cm_quantile cm;
    double quants[] = {0.5, 1, -2.0};
    int res = init_cm_quantile(0.01, (double*)&quants, 3, &cm);
    fail_unless(res == -1);
}
END_TEST

START_TEST(test_cm_init_bad_eps)
{
    cm_quantile cm;
    double quants[] = {0.5, 0.9, 0.99};
    int res = init_cm_quantile(1, (double*)&quants, 3, &cm);
    fail_unless(res == -1);
}
END_TEST

START_TEST(test_cm_init_add_destroy)
{

    cm_quantile cm;
    double quants[] = {0.5, 0.90, 0.99};
    int res = init_cm_quantile(0.01, (double*)&quants, 3, &cm);
    fail_unless(res == 0);

    res = cm_add_sample(&cm, 100.0);
    fail_unless(res == 0);

    res = destroy_cm_quantile(&cm);
    fail_unless(res == 0);
}
END_TEST

START_TEST(test_cm_init_add_loop_destroy)
{

    cm_quantile cm;
    double quants[] = {0.5, 0.90, 0.99};
    int res = init_cm_quantile(0.01, (double*)&quants, 3, &cm);
    fail_unless(res == 0);

    for (int i=0; i<1000; i++) {
        res = cm_add_sample(&cm, i);
        fail_unless(res == 0);
    }

    res = destroy_cm_quantile(&cm);
    fail_unless(res == 0);
}
END_TEST

START_TEST(test_cm_init_query_destroy)
{

    cm_quantile cm;
    double quants[] = {0.5, 0.90, 0.99};
    int res = init_cm_quantile(0.01, (double*)&quants, 3, &cm);
    fail_unless(res == 0);

    double val = cm_query(&cm, 0.5);
    fail_unless(val == 0.0);

    res = destroy_cm_quantile(&cm);
    fail_unless(res == 0);
}
END_TEST

START_TEST(test_cm_init_add_query_destroy)
{
    cm_quantile cm;
    double quants[] = {0.5, 0.90, 0.99};
    int res = init_cm_quantile(0.01, (double*)&quants, 3, &cm);
    fail_unless(res == 0);

    res = cm_add_sample(&cm, 100.0);
    fail_unless(res == 0);

    double val = cm_query(&cm, 0.5);
    fail_unless(val == 100.0);

    res = destroy_cm_quantile(&cm);
    fail_unless(res == 0);
}
END_TEST

void print_cm(cm_quantile *cm) {
    cm_sample *samp = cm->samples;
    while (samp) {
        printf("%f - %lld %lld\n", samp->value, samp->width, samp->delta);
        samp = samp->next;
    }
}

START_TEST(test_cm_init_add_loop_query_destroy)
{
    cm_quantile cm;
    double quants[] = {0.5, 0.90, 0.99};
    int res = init_cm_quantile(0.01, (double*)&quants, 3, &cm);
    fail_unless(res == 0);

    for (int i=0; i < 100000; i++) {
        res = cm_add_sample(&cm, i);
        fail_unless(res == 0);
    }

    res = cm_flush(&cm);
    fail_unless(res == 0);

    double val = cm_query(&cm, 0.5);
    fail_unless(val >= 50000.0 - 1000 && val <= 50000 + 1000);

    res = destroy_cm_quantile(&cm);
    fail_unless(res == 0);
}
END_TEST

START_TEST(test_cm_init_add_loop_rev_query_destroy)
{
    cm_quantile cm;
    double quants[] = {0.5, 0.90, 0.99};
    int res = init_cm_quantile(0.01, (double*)&quants, 3, &cm);
    fail_unless(res == 0);

    for (int i=100000; i > 0; i--) {
        res = cm_add_sample(&cm, i);
        fail_unless(res == 0);
    }

    res = cm_flush(&cm);
    fail_unless(res == 0);

    double val = cm_query(&cm, 0.5);
    fail_unless(val >= 50000.0 - 1000 && val <= 50000 + 1000);

    res = destroy_cm_quantile(&cm);
    fail_unless(res == 0);
}
END_TEST

START_TEST(test_cm_init_add_loop_random_query_destroy)
{
    cm_quantile cm;
    double quants[] = {0.5, 0.90, 0.99};
    int res = init_cm_quantile(0.01, (double*)&quants, 3, &cm);
    fail_unless(res == 0);

    srandom(42);
    for (int i=0; i < 100000; i++) {
        res = cm_add_sample(&cm, random());
        fail_unless(res == 0);
    }

    res = cm_flush(&cm);
    fail_unless(res == 0);

    double val = cm_query(&cm, 0.5);
    fail_unless(val >= 1073741823 - 21474836 && val <= 1073741823 + 21474836);

    res = destroy_cm_quantile(&cm);
    fail_unless(res == 0);
}
END_TEST

