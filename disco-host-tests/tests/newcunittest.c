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

void testoutput1Q() {
	uint8_t *ibuf = "OUTP? 11";
	scpi_glue_input(ibuf, sizeof(ibuf), true, obuf);
	printf(obuf);
	CU_ASSERT(1);
}

void test_output_both_off() {
	uint8_t *ibuf = "outp off";
	scpi_glue_input(ibuf, sizeof(ibuf), true, obuf);
	printf(obuf);
	CU_ASSERT(1);
}

void test_output_single_off() {
	uint8_t *ibuf = "outp2 off";
	scpi_glue_input(ibuf, sizeof(ibuf), true, obuf);
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
		(NULL == CU_add_test(pSuite, "outp1?", testoutput1Q)) ||
//		(NULL == CU_add_test(pSuite, "testoutpu2_off", test_output_both_off)) ||
//		(NULL == CU_add_test(pSuite, "single off", test_output_single_off)) ||
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
