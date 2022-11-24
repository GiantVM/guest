// SPDX-License-Identifier: GPL-2.0
/*
 * Simple program to generate defines out of facility lists that use the bit
 * numbering scheme from the Princples of Operations: most significant bit
 * has bit number 0.
 *
 *    Copyright IBM Corp. 2015, 2018
 *
 */

#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct facility_def {
	char *name;
	int *bits;
};

static struct facility_def facility_defs[] = {
	{
		/*
		 * FACILITIES_ALS contains the list of facilities that are
		 * required to run a kernel that is compiled e.g. with
		 * -march=<machine>.
		 */
		.name = "FACILITIES_ALS",
		.bits = (int[]){
#ifdef CONFIG_HAVE_MARCH_Z900_FEATURES
			0,  /* N3 instructions */
			1,  /* z/Arch mode installed */
#endif
#ifdef CONFIG_HAVE_MARCH_Z990_FEATURES
			18, /* long displacement facility */
#endif
#ifdef CONFIG_HAVE_MARCH_Z9_109_FEATURES
			21, /* extended-immediate facility */
			25, /* store clock fast */
#endif
#ifdef CONFIG_HAVE_MARCH_Z10_FEATURES
			27, /* mvcos */
			32, /* compare and swap and store */
			33, /* compare and swap and store 2 */
			34, /* general instructions extension */
			35, /* execute extensions */
#endif
#ifdef CONFIG_HAVE_MARCH_Z196_FEATURES
			45, /* fast-BCR, etc. */
#endif
#ifdef CONFIG_HAVE_MARCH_ZEC12_FEATURES
			49, /* misc-instruction-extensions */
			52, /* interlocked facility 2 */
#endif
#ifdef CONFIG_HAVE_MARCH_Z13_FEATURES
			53, /* load-and-zero-rightmost-byte, etc. */
#endif
#ifdef CONFIG_HAVE_MARCH_Z14_FEATURES
			58, /* miscellaneous-instruction-extension 2 */
#endif
			-1 /* END */
		}
	},
	{
		/*
		 * FACILITIES_KVM contains the list of facilities that are part
		 * of the default facility mask and list that are passed to the
		 * initial CPU model. If no CPU model is used, this, together
		 * with the non-hypervisor managed bits, is the maximum list of
		 * guest facilities supported by KVM.
		 */
		.name = "FACILITIES_KVM",
		.bits = (int[]){
			0,  /* N3 instructions */
			1,  /* z/Arch mode installed */
			2,  /* z/Arch mode active */
			3,  /* DAT-enhancement */
			4,  /* idte segment table */
			5,  /* idte region table */
			6,  /* ASN-and-LX reuse */
			7,  /* stfle */
			8,  /* enhanced-DAT 1 */
			9,  /* sense-running-status */
			10, /* conditional sske */
			13, /* ipte-range */
			14, /* nonquiescing key-setting */
			73, /* transactional execution */
			75, /* access-exception-fetch/store indication */
			76, /* msa extension 3 */
			77, /* msa extension 4 */
			78, /* enhanced-DAT 2 */
			130, /* instruction-execution-protection */
			131, /* enhanced-SOP 2 and side-effect */
			139, /* multiple epoch facility */
			146, /* msa extension 8 */
			-1  /* END */
		}
	},
	{
		/*
		 * FACILITIES_KVM_CPUMODEL contains the list of facilities
		 * that can be enabled by CPU model code if the host supports
		 * it. These facilities are not passed to the guest without
		 * CPU model support.
		 */

		.name = "FACILITIES_KVM_CPUMODEL",
		.bits = (int[]){
			12, /* AP Query Configuration Information */
			15, /* AP Facilities Test */
			156, /* etoken facility */
			-1  /* END */
		}
	},
};

static void print_facility_list(struct facility_def *def)
{
	unsigned int high, bit, dword, i;
	unsigned long long *array;

	array = calloc(1, 8);
	if (!array)
		exit(EXIT_FAILURE);
	high = 0;
	for (i = 0; def->bits[i] != -1; i++) {
		bit = 63 - (def->bits[i] & 63);
		dword = def->bits[i] / 64;
		if (dword > high) {
			array = realloc(array, (dword + 1) * 8);
			if (!array)
				exit(EXIT_FAILURE);
			memset(array + high + 1, 0, (dword - high) * 8);
			high = dword;
		}
		array[dword] |= 1ULL << bit;
	}
	printf("#define %s ", def->name);
	for (i = 0; i <= high; i++)
		printf("_AC(0x%016llx,UL)%c", array[i], i < high ? ',' : '\n');
	free(array);
}

static void print_facility_lists(void)
{
	unsigned int i;

	for (i = 0; i < sizeof(facility_defs) / sizeof(facility_defs[0]); i++)
		print_facility_list(&facility_defs[i]);
}

int main(int argc, char **argv)
{
	printf("#ifndef __ASM_S390_FACILITIES__\n");
	printf("#define __ASM_S390_FACILITIES__\n");
	printf("/*\n");
	printf(" * DO NOT MODIFY.\n");
	printf(" *\n");
	printf(" * This file was generated by %s\n", __FILE__);
	printf(" */\n\n");
	printf("#include <linux/const.h>\n\n");
	print_facility_lists();
	printf("\n#endif\n");
	return 0;
}
