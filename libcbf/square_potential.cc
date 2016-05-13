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

#include <cbf/square_potential.h>
#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>

namespace CBF {

	void SquarePotential::gradient (
		FloatVector &result,
		const std::vector<FloatVector > &references,
		const FloatVector &input) {
		// First we find the closest reference vector
		Float min_dist = std::numeric_limits<Float>::max();
		unsigned int min_index = 0;

		for (unsigned int i = 0; i < references.size(); ++i) {
			Float dist = distance(input, references[i]);
			if (dist < min_dist) {
				min_index = i;
				min_dist = dist;
			}
		}

		m_CurrentReference = references[min_index];

		result = (m_CurrentReference - input);
	}

	void SquarePotential::integration (
		FloatVector &nextpos,
		const FloatVector &currentpos,
		const FloatVector &taskvel,
		const Float timestep)
	{
		nextpos = currentpos + taskvel*timestep;
	}


#ifdef CBF_HAVE_XSD
	SquarePotential::SquarePotential(const CBFSchema::SquarePotential &xml_instance, ObjectNamespacePtr object_namespace) :
		Potential(xml_instance, object_namespace) 
	{
		CBF_DEBUG("[SquarePotential(const SquaredPotentialType &xml_instance)]: yay!");
		CBF_DEBUG("Coefficient: " << xml_instance.Coefficient());
		m_Coefficient = xml_instance.Coefficient();

		m_SensorDim = xml_instance.Dimension();

		// m_DistanceThreshold = xml_instance.DistanceThreshold();
	}

	static XMLDerivedFactory<SquarePotential, CBFSchema::SquarePotential> x;

	static XMLConstructorCreator<
		Potential, 
		SquarePotential,
		CBFSchema::SquarePotential
	> foobar;

	//static XMLConstructorCreator<Potential, CBFSchema::SquarePotential> x3;

#endif
} // namespace
