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

#include "ata.h"
#include <assert.h>
#include <memory.h>

bool ata_inq_checksum(unsigned char *buf, int buf_len)
{
	char sum;
	int idx;

	if (buf[511] != 0xA5) {
		// Checksum isn't claimed to be valid, nothing to check here
		return 1;
	}

	for (idx = 0, sum = 0; idx < buf_len; idx++) {
		sum += buf[idx];
	}

	return sum == 0;
}

bool ata_status_from_scsi_sense(unsigned char *sense, int sense_len, ata_status_t *status)
{
        sense_info_t sense_info;

	assert(status);

        bool parsed = scsi_parse_sense(sense, sense_len, &sense_info);

        if (parsed && sense_info.ata_status_valid) {
                *status = sense_info.ata_status;
                return true;
        }
        if (sense_info.is_fixed) {
                // Fixed format parsing for ATA passthrough can't be known automatically
                memset(status, 0, sizeof(*status));
                status->error = sense_info.information & 0xFF;
                status->status = (sense_info.information >> 8) & 0xFF;
                status->device = (sense_info.information >> 16) & 0xFF;

                status->sector_count = (sense_info.information >> 24) & 0xFF;

                status->extend = sense_info.cmd_specific & 0x80;
                uint32_t lba_high = (sense_info.cmd_specific >> 8) & 0xFF;
                uint32_t lba_mid = (sense_info.cmd_specific >> 16) & 0xFF;
                uint32_t lba_low = (sense_info.cmd_specific >> 24) & 0xFF;
                status->lba = (lba_high << 16) | (lba_mid << 8) | lba_low;

                // TODO: sector_count_upper_non_zero: sense_info.cmd_specific & 0x40
                // TODO: lba upper non zero: sense_info.cmd_specific & 0x20
                // TODO: log index: sense_info.cmd_specific & 0x07
        }

        return false;
}
