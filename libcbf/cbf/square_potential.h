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

/* -*- mode: c-non-suck; -*- */

#ifndef CONTROL_SQUARE_POTENTIAL_HH
#define CONTROL_SQUARE_POTENTIAL_HH

#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/potential.h>
#include <cbf/config.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class SquarePotential; }

namespace CBF {

/**
	@brief A squared potential functions
*/
struct SquarePotential : public Potential {
	SquarePotential(const CBFSchema::SquarePotential &xml_instance, ObjectNamespacePtr object_namespace);

	Float m_Coefficient;

	unsigned int m_Dim;

	SquarePotential(unsigned int dim = 1, Float coefficient = 1.) :
		m_Coefficient(coefficient),
		m_Dim(dim)
	{
	}

	virtual Float norm(const FloatVector &v) {
		return v.norm();
	}

	virtual Float distance(const FloatVector &v1, const FloatVector &v2) {
		return (norm(v1 - v2));
	}

	virtual unsigned int dim() const {
		return m_Dim;
	}

	virtual void gradient (
		FloatVector &result,
		const std::vector<FloatVector > &references,
		const FloatVector &input
	);
};

typedef boost::shared_ptr<SquarePotential> SquarePotentialPtr;

} // namespace

#endif

