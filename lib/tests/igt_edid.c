/*
 * Copyright © 2019 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors: Simon Ser <simon.ser@intel.com>
 */

#include "config.h"

#include <stdbool.h>

#include "igt_core.h"
#include "igt_kms.h"
#include "igt_edid.h"

static const unsigned char edid_header[] = {
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00
};

/**
 * Sanity check the header of the base EDID block.
 */
static bool edid_header_is_valid(const unsigned char *raw_edid)
{
	size_t i;

	for (i = 0; i < sizeof(edid_header); i++)
		if (raw_edid[i] != edid_header[i])
			return false;

	return true;
}

/**
 * Sanity check the checksum of the EDID block.
 */
static bool edid_block_checksum(const unsigned char *raw_edid)
{
	size_t i;
	unsigned char csum = 0;

	for (i = 0; i < EDID_LENGTH; i++) {
		csum += raw_edid[i];
	}

	return csum == 0;
}

typedef const unsigned char *(*get_edid_func)(void);

igt_simple_main
{
	const struct {
		const char *desc;
		get_edid_func f;
		size_t exts;
	} funcs[] = {
		{ "base", igt_kms_get_base_edid, 0 },
		{ "alt", igt_kms_get_alt_edid, 0 },
		{0},
	}, *f;
	const unsigned char *edid;
	size_t i;

	for (f = funcs; f->f; f++) {
		edid = f->f();

		igt_assert_f(edid_header_is_valid(edid),
			     "invalid header on %s EDID", f->desc);
		/* check base edid block */
		igt_assert_f(edid_block_checksum(edid),
			     "checksum failed on %s EDID", f->desc);
		/* check extension blocks, if any */
		for (i = 0; i < f->exts; i++)
			igt_assert_f(edid_block_checksum(edid + (i + 1) * EDID_LENGTH),
				     "CEA block checksum failed on %s EDID", f->desc);
	}
}
