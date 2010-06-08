/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2009, 2010 Florian Paul Schmidt
*/

#ifndef CBF_AXIS_ANGLE_ORIENTATION_POTENTIALS_HH
#define CBF_AXIS_ANGLE_ORIENTATION_POTENTIALS_HH

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/potential.h>
#include <cbf/plugin_decl_macros.h>

#include <boost/shared_ptr.hpp>

CBF_PLUGIN_PREAMBLE(AxisAnglePotential)

namespace CBF {
	namespace ublas = boost::numeric::ublas;
	
	/**
		@brief A potential function operating on the space of rotations...
	
		represented as Axis Angle, but with all information encoded into the
		direction and length of the axis.
	*/
	struct AxisAnglePotential : public Potential {
		CBF_PLUGIN_DECL_METHODS(AxisAnglePotential)
		//! @brief  This coefficient determines the rate of convergence. Choose between 0 and 1.
		Float m_Coefficient;

		bool m_Converged;
	
		AxisAnglePotential(Float coefficient = 0.1) :
			m_Coefficient(coefficient)
		{
	
		}
	
		virtual void gradient (
			FloatVector &result, 
			const std::vector<FloatVector > &references, 
			const FloatVector &input
		);
	
		virtual unsigned int dim() const { return 3u; }

		virtual bool converged() const {
			return m_Converged;
		}

		virtual Float distance(const FloatVector &v1, const FloatVector &v2);

		virtual Float norm(const FloatVector &v) {
			return ublas::norm_2(v);
		}
	};

	typedef boost::shared_ptr<AxisAnglePotential> AxisAnglePotentialPtr;

} // namespace

#endif


