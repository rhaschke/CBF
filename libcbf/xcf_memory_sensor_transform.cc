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


    Copyright 2009, 2010 Florian Paul Schmidt, Viktor Richter
*/

/* -*- mode: c-non-suck; -*- */

#include <cbf/xcf_memory_sensor_transform.h>
#include <cbf/debug_macros.h>
#include <cbf/schemas.hxx>

namespace CBF {

	XCFMemorySensorTransform::XCFMemorySensorTransform(
					const std::string &uri, 
					const std::string &result_name, 
					SensorTransformPtr sensor_transform
					)
	{
		m_MemoryPtr = memory::interface::MemoryInterface::getInstance(uri);
		m_ResultName = result_name;
		m_SensorTransform = sensor_transform;
	}

	void XCFMemorySensorTransform::update(const FloatVector &resource_value){
		m_SensorTransform -> update(resource_value);
		send();
	}

	void XCFMemorySensorTransform::send(){

		//Getting the result vector from the SensorTransform.
		FloatVector result = m_SensorTransform -> result();
		std::stringstream vector_string;

		//Creating an XML-document.
		CBF_DEBUG("creating vector string")
		vector_string << "[" << result.size() << "](";
		for (unsigned int i = 0; i < result.size(); ++i) {
			vector_string << result(i);
			if ( i !=  result.size() -1 ) vector_string << ",";
		}
		vector_string << ")";

		CBF_DEBUG("creating vector doc")

		CBFSchema::BoostVector vectorDoc(vector_string.str());

		CBF_DEBUG("creating transformResult doc")
		CBFSchema::XCFMemorySensorTransformResult resultDoc(m_ResultName, vectorDoc);

		std::ostringstream s;
		CBFSchema::XCFMemorySensorTransformResult_ (s, resultDoc);

		CBF_DEBUG("document: " << s.str())


		//Sending the result-XML to the server.
		CBF_DEBUG("sending result to memory-server")
		
		m_MemoryPtr -> send(s.str());

		//Getting the task-jacobian matrix from the SensorTransform.
		FloatMatrix task_jacobian = m_SensorTransform -> task_jacobian();
		
		//Creating an XML-document.
		std::stringstream matrix_string;

		unsigned int i,j,m,n;
		m = task_jacobian.size1();
		n = task_jacobian.size2();

		CBF_DEBUG("creating matrix string")
		matrix_string << "[" << m << "," << n << "](";
		for (i = 0; i < m ; ++i) {
			for (j = 0; j < n ; j++){
				matrix_string << task_jacobian(i,j);
				if (i != m - 1 && j != n -1 ) matrix_string << ",";
			}
		}
		matrix_string << ")";

//		CBF_DEBUG("creating matrix doc")

//		CBFSchema::BoostMatrix matrixDoc(matrix_string.str());

//		CBF_DEBUG("creating transformResult doc")
//		CBFSchema::XCFMemorySensorTransformResult taskJacobianDoc(resultName, matrixDoc);

//		std::ostringstream t;
//		CBFSchema::XCFMemorySensorTransformResult_ (t, taskJacobianDoc);

//		CBF_DEBUG("document: " << t.str())


		//Sending the result-XML to the server.
//		CBF_DEBUG("sending result to memory-server")
		
//		m_MemoryPtr -> send(t.str());
}

} // namespace
