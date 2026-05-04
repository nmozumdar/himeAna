/*
	This file was taken from the Stream framework.

	----------------------------------------------------------------
		                  The Stream framework
	----------------------------------------------------------------
	Copyright (C) 2013 -
	GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
	Planckstr. 1, 64291 Darmstadt, Germany
	Contact:  https://github.com/gsi-ee/stream
	----------------------------------------------------------------
	DISCLAIMER:
	The Stream framework is free software: you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation, either version 3
	of the License, or (at your option) any later version.

	The Stream framework is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
	the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the Stream framework. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef HADAQ_SCALERSUBEVENT_H
#define HADAQ_SCALERSUBEVENT_H

#include "SubEvent.h"

namespace hadaq {

	/** \brief Output float message for Scaler
     *
     * \ingroup stream_hadaq_classes */

	struct ScalerSubEvent : public base::SubEvent {
		bool valid = true;
		uint64_t scaler1 = 0;    ///< scaler 1
		uint64_t scaler2 = 0;    ///< scaler 1

		ScalerSubEvent() = default;

		ScalerSubEvent(uint64_t s1, uint64_t s2, bool v = true)
		{
			valid = v;
			scaler1 = s1;
			scaler2 = s2;
		}

		/** clear sub event */
		void Clear() override
		{
			valid = true;
			scaler1 = 0;
			scaler2 = 0;
		}
		ClassDef(hadaq::ScalerSubEvent,1)
	};
}

#endif
