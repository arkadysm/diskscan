/* Copyright 2013 Baruch Even
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "scsicmd.h"

#include <memory.h>

static inline void set_uint16(unsigned char *cdb, int start, uint16_t val)
{
        cdb[start] = (val >> 8) & 0xFF;
        cdb[start+1] = val & 0xFF;
}

static inline void set_uint32(unsigned char *cdb, int start, uint32_t val)
{
        cdb[start]   = (val >> 24) & 0xFF;
        cdb[start+1] = (val >> 16) & 0xFF;
        cdb[start+2] = (val >> 8) & 0xFF;
        cdb[start+3] = val & 0xFF;
}

static inline void set_uint64(unsigned char *cdb, int start, uint64_t val)
{
        cdb[start]   = (val >> 56) & 0xFF;
        cdb[start+1] = (val >> 48) & 0xFF;
        cdb[start+2] = (val >> 40) & 0xFF;
        cdb[start+3] = (val >> 32) & 0xFF;
        cdb[start+4] = (val >> 24) & 0xFF;
        cdb[start+5] = (val >> 16) & 0xFF;
        cdb[start+6] = (val >>  8) & 0xFF;
        cdb[start+7] = val & 0xFF;
}

int cdb_tur(unsigned char *cdb)
{
	const int TUR_LEN = 6;
	memset(cdb, 0, TUR_LEN);
	return TUR_LEN;
}

int cdb_inquiry(unsigned char *cdb, bool evpd, char page_code, uint16_t alloc_len)
{
	const int INQUIRY_LEN = 6;

	cdb[0] = 0x12;
	cdb[1] = evpd ? 1 : 0;
	cdb[2] = page_code;
	set_uint16(cdb, 3, alloc_len);
	cdb[5] = 0;

	return INQUIRY_LEN;
}
