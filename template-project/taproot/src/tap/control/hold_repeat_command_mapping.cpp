/*
 * Copyright (c) 2020-2021 Advanced Robotics at the University of Washington <robomstr@uw.edu>
 *
 * This file is part of Taproot.
 *
 * Taproot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Taproot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Taproot.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "hold_repeat_command_mapping.hpp"

#include "tap/drivers.hpp"

namespace tap
{
namespace control
{
void HoldRepeatCommandMapping::executeCommandMapping(const RemoteMapState &currState)
{
    // if mapping subset of the current remote map state and if the neg keys are not used or the neg
    // keys are not pressed, schedule or reschedule command
    // see `RemoteMapState` class comment if confused about neg keys
    if (mappingSubset(currState) &&
        !(mapState.getNegKeysUsed() && negKeysSubset(mapState, currState)))
    {
        for (std::size_t i = 0; i < mappedCommands.size(); i++)
        {
            Command *cmd = mappedCommands[i];
            if (!drivers->commandScheduler.isCommandScheduled(cmd))
            {
                if (okToScheduleCommand(i))
                {
                    drivers->commandScheduler.addCommand(cmd);
                    incrementRescheduleCount(i);
                }
            }
        }
        held = true;
    }
    else
    {
        // remove commands if the commands were previously scheduled and we actually want to end
        // commands when not held
        if (held)
        {
            held = false;

            if (endCommandsWhenNotHeld)
            {
                removeCommands();
            }

            for (std::size_t i = 0; i < rescheduleCounts.size(); i++)
            {
                rescheduleCounts[i] = 0;
            }
        }
    }
}
}  // namespace control
}  // namespace tap
