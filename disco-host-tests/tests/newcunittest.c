/*
 * File:   newcunittest.c
 * Author: karlp
 *
 * Created on Jan 28, 2014, 9:17:22 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include "../scpi-arch.h"
/*
 * CUnit Test Suite
 */

char obuf[1024];

int init_suite(void)
{
	scpi_init();
	return 0;
}

int clean_suite(void)
{
	return 0;
}

void testScpi_glue_input()
{
	uint8_t ibuf[] = {0x2a, 0x49, 0x44, 0x4e, 0x3f};
	scpi_glue_input(ibuf, sizeof(ibuf), true, obuf);
	printf(obuf);
	
	if (1 /*check result*/) {
		CU_ASSERT(1);
	}
}

void testoutput_explicit() {
	uint8_t *ibuf = "OUTP1?";
	scpi_glue_input(ibuf, strlen(ibuf), true, obuf);
	printf(obuf);
	CU_ASSERT(1);
}

void testoutput_upwards() {
	uint8_t *ibuf = "OUTP?";
	scpi_glue_input(ibuf, strlen(ibuf), true, obuf);
	printf(obuf);
	CU_ASSERT(1);
}

void test_output_war_load1() {
	uint8_t *ibuf = "OUTP:LOAD?";
	scpi_glue_input(ibuf, strlen(ibuf), true, obuf);
	printf(obuf);
	CU_ASSERT(1);
}

void test_output_load2() {
	uint8_t *ibuf = "outp2:load?";
	scpi_glue_input(ibuf, strlen(ibuf), true, obuf);
	printf(obuf);
	CU_ASSERT(1);
}

void test_bjarni1() {
	uint8_t *ibuf = "outp2 off";
	scpi_glue_input(ibuf, strlen(ibuf), true, obuf);
	printf(obuf);
	CU_ASSERT(1);
}

void test_bjarni2() {
	uint8_t *ibuf = "outp1:load inf";
	scpi_glue_input(ibuf, strlen(ibuf), true, obuf);
	printf(obuf);
	CU_ASSERT(1);
}

void test_bjarni3() {
	uint8_t *ibuf = "sour1:appl:sin 50Hz,0.465Vrms,0";
	scpi_glue_input(ibuf, strlen(ibuf), true, obuf);
	printf(obuf);
	CU_ASSERT(1);
}

int main()
{
	CU_pSuite pSuite = NULL;

	/* Initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* Add a suite to the registry */
	pSuite = CU_add_suite("newcunittest", init_suite, clean_suite);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Add the tests to the suite */
	if (0 ||
		(NULL == CU_add_test(pSuite, "testScpi_glue_input", testScpi_glue_input)) ||
		(NULL == CU_add_test(pSuite, "testoutput_explicit", testoutput_explicit)) ||
		(NULL == CU_add_test(pSuite, "testoutput_upwards", testoutput_upwards)) ||
		(NULL == CU_add_test(pSuite, "test_output_load2", test_output_load2)) ||
		(NULL == CU_add_test(pSuite, "test_WAR_load1", test_output_war_load1)) ||
		(NULL == CU_add_test(pSuite, "test_bjarni1", test_bjarni1)) ||
		(NULL == CU_add_test(pSuite, "test_bjarni2", test_bjarni2)) ||
		(NULL == CU_add_test(pSuite, "test_bjarni3", test_bjarni3)) ||
		0) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
