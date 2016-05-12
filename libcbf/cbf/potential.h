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

#ifndef CONTROL_BASIS_POTENTIAL_HH
#define CONTROL_BASIS_POTENTIAL_HH

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/debug_macros.h>
#include <cbf/object.h>
#include <cbf/namespace.h>

#include <vector>
#include <boost/shared_ptr.hpp>

namespace CBFSchema { class Potential; }

namespace CBF {


/**
	@brief This class represents an artificial potential function.

	The most important part of it is the gradient function. The 
	exec function ist just there for convenience in case the
	absolute value of the potential function needs to be known.
*/
struct Potential : public Object {
	bool m_ClampGradientStepNorm;
	Float m_MaxGradientStepNorm;

	Potential(const CBFSchema::Potential &xml_instance, ObjectNamespacePtr object_namespace);

	Potential(
		Float max_gradient_step_norm = 0.1
	) :
		Object("Potential"),
		m_MaxGradientStepNorm(max_gradient_step_norm)
	{

	}

	virtual ~Potential() {

	}

	virtual void set_max_gradient_step_norm(Float gradient_norm) {
		m_MaxGradientStepNorm = gradient_norm;
	}

	virtual Float max_gradient_step_norm() {
		return m_MaxGradientStepNorm;
	}

	/**
		The norm() function has to return a sensible notion of length for the
		gradient step that is passed into it..
	*/
	virtual Float norm(const FloatVector &v) = 0;

	/**
		For two points of the space that this potential is operating on, this function
		should return a sensible notion of distance between the two points
	*/
	virtual Float distance(const FloatVector &v1, const FloatVector &v2) = 0;

	/**
		The gradient can be implemented analytically or 
		numerically depending on taste and application
	*/
	virtual void gradient (
		FloatVector &result, 
		const std::vector<FloatVector > &references, 
		const FloatVector &input
	) = 0;

	/**
		The integration function
	*/
	virtual void integration (
		FloatVector &nextpos,
		const FloatVector &currentpos,
		const FloatVector &currentgradient,
		const Float timestep
	) = 0;

	/**
		Dimension
	*/
	virtual unsigned int dim() const = 0;

	/**
		Dimension in gradient level
	*/
	virtual unsigned int dim_grad() const = 0;
};

typedef boost::shared_ptr<Potential> PotentialPtr;




} // namespace

#endif

