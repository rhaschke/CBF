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

#ifndef CBF_DUMMY_REFERENCE_HH
#define CBF_DUMMY_REFERENCE_HH

#include <cbf/reference.h>
#include <cbf/types.h>
#include <cbf/exceptions.h>
#include <cbf/namespace.h>

#include <stdexcept>

namespace CBFSchema { class DummyReference; }

namespace CBF {

	struct DummyReference : public Reference {
		DummyReference(const CBFSchema::DummyReference &xml_instance, 
							ObjectNamespacePtr object_namespace);

		DummyReference(unsigned int num_references = 1, unsigned int dim = 1)
		{
			if (num_references < 1)
				CBF_THROW_RUNTIME_ERROR("num_references < 1");

			if (dim < 1)
				CBF_THROW_RUNTIME_ERROR("dim < 1");

			m_References = std::vector<FloatVector>(num_references, FloatVector(dim));
			for (unsigned int i = 0; i < num_references; ++i)
				for (unsigned int j = 0; j < dim; ++j)
					m_References[i][j] = 0;
		}

		virtual void update() { }

		virtual std::vector<FloatVector> &references() {
			return m_References;
		}

		virtual unsigned int dim() {
			return m_References[0].size();
		}

		protected:
	};

	typedef boost::shared_ptr<DummyReference> DummyReferencePtr;
} // namespace

#endif
