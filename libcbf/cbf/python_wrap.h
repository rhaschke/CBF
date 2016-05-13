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

#ifndef CBF_PYTHON_WRAP_HH
#define CBF_PYTHON_WRAP_HH

#include <cbf/sensor_transform.h>
#include <cbf/effector_transform.h>
#include <cbf/potential.h>

namespace CBFSchema {
	class PythonPotential;
	class PythonSensorTransform;
}

namespace CBF {
	
	/**
		@brief This class is used in all Python wrappers to transparently 
		start the interpreter.
	*/
	struct PythonInterpreter;
	
	struct PythonPotential;
	typedef boost::shared_ptr<PythonPotential> PythonPotentialPtr;
	
	
	/**
		@brief A potential class that calls a python script to do the heavy lifting.
	
		This class is probably not thread-safe. Do not use two instances of the same
		python script..
	
		\todo Implement m_InitScript and m_FiniScript support..
	
		The exec() implementation of this function executes the m_ExecScript using the 
		python interpreter. See the PythonPotential::m_ExecScript documentation for "calling
		conventions".
	
		Keep in mind that python is very sensitive to whitespaces, thus
		there exists a method to "sanitize" scripts which is useful when
		used in XML files:
	
		<PRE>
		&lt;Controller xsi:type="PrimitiveControllerType"&gt;
		&lt;Potential xsi:type="PythonPotentialType"&gt;
		&lt;Dimension&gt; 3 &lt;/Dimension&gt;
		&lt;ExecScript&gt;
		#---
		import scipy as S
		import scipy.linalg as L
		ref = S.array(references[0])
		inp = S.array(input)
		res = 0.1 * (ref - inp)
		result = list(res)
		#---
		&lt;/ExecScript&gt;
		&lt;/Potential&gt;
		...
		&lt;/Controller&gt;
		</PRE>
	
		As you can see, the script is wrapped into two comments of the form "#---".
		All characters outside these two comments are stripped. There must not be any
		additional characters on the two comment lines.
	*/
	struct PythonPotential : public Potential {
		PythonPotential(const CBFSchema::PythonPotential &xml_instance,
							 ObjectNamespacePtr object_namespace);

		protected:
			const PythonInterpreter &m_Interpreter;
      unsigned int m_SensorDim;
      unsigned int m_TaskDim;
	
		public:
      PythonPotential(unsigned int dim = 1, unsigned int dim_grad = 1);
	
			virtual Float norm(const FloatVector &v) { return 0; }

			virtual Float distance(const FloatVector &v1, const FloatVector &v2) { return 0; }

			/** TODO: implement!! */
			virtual bool converged() const { return false; }

      virtual void gradient (FloatVector &result,
        const std::vector<FloatVector > &references,
        const FloatVector &input);

      virtual void integration (FloatVector &nextpos,
          const FloatVector &currentpos,
          const FloatVector &currentvel,
          const Float timestep);
		
      virtual unsigned int sensor_dim() const;

      virtual unsigned int task_dim() const;
	
			std::string m_InitScript;
	
			/**
				The m_ExecCode member has to contain a script that expects
				a list of lists of floats named "references" which holds the references,
				a list of floats named "input" which holds the input, and
				writes a list of floats called "result" which holds
				the result.
			*/
			std::string m_ExecScript;
	
			std::string m_FiniScript;
	};
	
	struct PythonSensorTransform;
	typedef boost::shared_ptr<PythonSensorTransform> PythonSensorTransformPtr;
	
	
	/**
		@brief A SensorTransform that calls a python script to do the heavy lifting.
	
		This class is probably not thread-safe. Do not use two instances of the same
		python script..
	
		Please note the comments for PythonPotential regarding the sanitizing of python
		scripts.
	
		See the PythonSensorTransform::m_ExecScript documentation for conventions about
		naming the arguments and results..
	
		\todo Implement m_InitScript and m_FiniScript support..
	*/
	struct PythonSensorTransform : public SensorTransform {
		protected:
			const PythonInterpreter &m_Interpreter;
		
		public:
			PythonSensorTransform(
				unsigned int task_dim = 1,
				unsigned int resource_dim = 1
			);
#ifdef CBF_HAVE_XSD
			PythonSensorTransform(
				const CBFSchema::PythonSensorTransform &xml_instance,
				ObjectNamespacePtr object_namespace);
#endif

			virtual void update(const FloatVector &resource_value);
	
			std::string m_InitScript;
	
			/**
				The m_ExecCode member has to contain a script that expects
				a list of floats named "resource" which holds the current
				resource values, and writes a list of floats called "result" which holds
				the result and a list of list of floats called "jacobian" which
				holds the current jacobian matrix.
			*/
			std::string m_ExecScript;
	
			std::string m_FiniScript;
	};
} // namespace


#endif
