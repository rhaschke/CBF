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

#include <cbf/primitive_controller.h>
#include <cbf/types.h>
#include <cbf/debug_macros.h>
#include <cbf/exceptions.h>
#include <cbf/xml_factory.h>
#include <cbf/xml_object_factory.h>

#include <vector>
#include <iostream>
#include <cassert>


namespace CBF {
	SubordinateController::SubordinateController(Float alpha,
		std::vector<ConvergenceCriterionPtr> convergence_criteria,
		ReferencePtr reference,
		PotentialPtr potential,
		TaskSpacePlannerPtr planner,
		SensorTransformPtr sensor_transform,
		EffectorTransformPtr effector_transform,
		std::vector<SubordinateControllerPtr> subordinate_controllers,
		CombinationStrategyPtr combination_strategy )
	{
		init(
			alpha,
			convergence_criteria,
			reference,
			potential,
			planner,
			sensor_transform,
			effector_transform,
			subordinate_controllers,
			combination_strategy
		);

		CBF_DEBUG("init");
	}

	void SubordinateController::init(
		Float coefficient,
		std::vector<ConvergenceCriterionPtr> convergence_criteria,
		ReferencePtr reference,
		PotentialPtr potential,
		TaskSpacePlannerPtr planner,
		SensorTransformPtr sensor_transform,
		EffectorTransformPtr effector_transform,
		std::vector<SubordinateControllerPtr> subordinate_controllers,
		CombinationStrategyPtr combination_strategy
	) {
		m_Master = NULL;
		m_Coefficient = coefficient;
		m_ConvergenceCriteria = convergence_criteria;
		m_Reference = reference;
		m_Potential = potential;
		m_TaskSpacePlanner = planner,
		m_SensorTransform = sensor_transform;
		m_EffectorTransform = effector_transform;
		m_SubordinateControllers = subordinate_controllers;
		m_CombinationStrategy = combination_strategy;
		for (std::vector<SubordinateControllerPtr>::iterator 
				  it  = m_SubordinateControllers.begin(),
				  end = m_SubordinateControllers.end(); 
			  it != end; ++it) {
			(*it)->m_Master = this;
		}
	}

	void PrimitiveController::reset(void)
	{
		m_SensorTransform->update(m_Resource->get());

		m_TaskSpacePlanner->reset(m_SensorTransform->result(),
		                          m_SensorTransform->task_jacobian()*m_Resource->get_resource_step());

	}

	void PrimitiveController::reset(const FloatVector resource_value, const FloatVector resource_step)
	{
		m_Resource->update(resource_value, resource_step);
		reset();
	}

	void PrimitiveController::init(
			ResourcePtr resource
	) {
		m_Resource = resource;

		reset();
		check_dimensions();
	}

	PrimitiveController::PrimitiveController(Float alpha,
		std::vector<ConvergenceCriterionPtr> convergence_criteria,
		ReferencePtr reference,
		PotentialPtr potential,
		TaskSpacePlannerPtr planner,
		SensorTransformPtr sensor_transform,
		EffectorTransformPtr effector_transform,
		std::vector<SubordinateControllerPtr> subordinate_controllers,
		CombinationStrategyPtr combination_strategy,
		ResourcePtr resource
	)	:
		SubordinateController(
			alpha,
			convergence_criteria,
			reference,
			potential,
			planner,
			sensor_transform,
			effector_transform,
			subordinate_controllers,
			combination_strategy
		)
	{
		init(resource);

		CBF_DEBUG("init");
	}
	

	void SubordinateController::check_dimensions() {
		if (m_Reference->dim() != m_Potential->dim())
			CBF_THROW_RUNTIME_ERROR(m_Name << ": Reference and Potential dimensions mismatch: " << m_Reference->dim() << " is not equal to " << m_Potential->dim());

		if (m_SensorTransform->task_dim() != m_Potential->dim())
			CBF_THROW_RUNTIME_ERROR(m_Name << ": Sensor Transform and Potential dimension mismatch: " << m_SensorTransform->task_dim() << " is not equal to " << m_Potential->dim());

		if (m_SensorTransform->resource_dim() != m_EffectorTransform->resource_dim())
			CBF_THROW_RUNTIME_ERROR(m_Name << ": Sensor Transform and Effector transform resource dimension mismatch: " << m_SensorTransform->resource_dim() << " is not equal to " << m_EffectorTransform->resource_dim());

		if (m_SensorTransform->task_dim() != m_EffectorTransform->task_dim())
			CBF_THROW_RUNTIME_ERROR(m_Name << ": Sensor Transform and Effector transform task dimension mismatch: " << m_SensorTransform->task_dim() << " is not equal to " << m_EffectorTransform->task_dim());
	}	

	ResourcePtr SubordinateController::resource() { 
		return m_Master->resource(); 
	}	

	void PrimitiveController::update() {
		m_Resource->update();
		SubordinateController::update();
	}	
	
	void SubordinateController::update() 
	{
		assert(m_Reference.get() != 0);
		assert(m_SensorTransform.get() != 0);
		assert(m_EffectorTransform.get() != 0);
		assert(m_Potential.get() != 0);
		assert(m_TaskSpacePlanner.get() != 0);
		assert(m_CombinationStrategy.get() != 0);

		assert(m_Reference->dim() == m_Potential->dim());

		m_Reference->update();

		m_References = m_Reference->get();

		//! Fill vector with data from sensor transform
		m_SensorTransform->update(resource()->get());
		CBF_DEBUG("jacobian: " << std::endl << m_SensorTransform->task_jacobian());

		m_EffectorTransform->update(resource()->get(), m_SensorTransform->task_jacobian());
		CBF_DEBUG("inv. jacobian: " << std::endl << m_EffectorTransform->inverse_task_jacobian());
	
		m_CurrentTaskPosition = m_SensorTransform->result();
		CBF_DEBUG("currentTaskPosition: " << m_CurrentTaskPosition.transpose());
	
		if (m_References.size() != 0) {	
			CBF_DEBUG("have reference!");
			//! then we do the gradient step
		//m_Potential->gradient(m_GradientStep, m_References, m_CurrentTaskPosition, 1.0);

		m_TaskSpacePlanner->update(m_References);
		m_TaskSpacePlanner->get_task_step(m_GradientStep, m_CurrentTaskPosition);
			CBF_DEBUG("gradientStep: " << m_GradientStep);

			//! Map gradient step into resource step via exec:
			CBF_DEBUG("calling m_EffectorTransform->exec(): Type is: " << CBF_UNMANGLE(*m_EffectorTransform.get()));
			m_EffectorTransform->exec(m_GradientStep, m_ResourceStep);
		} else {
			m_ResourceStep = FloatVector::Zero(resource()->dim());
		}
	
		CBF_DEBUG("resourceStep: " << m_ResourceStep.transpose());

		//! then we recursively call subordinate controllers.. and gather their
		//! effector transformed gradient steps.
		m_SubordinateGradientSteps.resize(m_SubordinateControllers.size());
	
		for (unsigned int i = 0; i < m_SubordinateControllers.size(); ++i) {
			m_SubordinateControllers[i]->update();
			m_SubordinateGradientSteps[i] = m_SubordinateControllers[i]->result();
			CBF_DEBUG("subordinate_gradient_step: " << m_SubordinateGradientSteps[i]);
		}
	
		m_CombinedResults = FloatVector::Zero(resource()->dim());
	
		m_CombinationStrategy->exec(m_CombinedResults, m_SubordinateGradientSteps);
	
		//! finally the results of all subordinate controllers are projected
		//! into our nullspace.For this we need the task jacobian and its inverse. 
		//! We get these from the effector transforms.
		m_InvJacobianTimesJacobian = m_EffectorTransform->inverse_task_jacobian()
				* m_SensorTransform->task_jacobian();
	
		//! The projector is (1 - J# J), so this is result = result - (J# J result)
		//! which can be expressed as result -= ...
		m_CombinedResults -= m_InvJacobianTimesJacobian
			* m_CombinedResults;
		CBF_DEBUG("combined_results * beta: " << m_CombinedResults);
	
		m_Result = (m_ResourceStep * m_Coefficient) + m_CombinedResults;
	}
	
	void PrimitiveController::action() {
		m_Resource->add(m_Result);
		m_Converged = check_convergence();
	}
	
	bool SubordinateController::check_convergence() {
		m_Converged = false;

		for (unsigned int i = 0, max = m_ConvergenceCriteria.size(); i < max; ++i) {
			if (m_ConvergenceCriteria[i]->check_convergence(*this)) {
				m_Converged = true;
				break;
			}
		}
		m_Stalled = !m_Converged && m_Result.norm() < 0.005 * m_GradientStep.norm();
		return m_Converged;
	}
	
	bool SubordinateController::finished() {
		// check whether we have approached one of the references to within a small error
		return m_Converged;
	}
	bool SubordinateController::stalled() {
		return m_Stalled;
	}

	void PrimitiveController::check_dimensions() const {
		SubordinateController::check_dimensions();

		if (m_SensorTransform->resource_dim() != m_Resource->dim())
			CBF_THROW_RUNTIME_ERROR(m_Name + ": Sensor Transform and Resource dimension mismatch: " << m_SensorTransform->resource_dim() << " is not equal to " << m_Resource->dim());

		if (m_EffectorTransform->resource_dim() != m_Resource->dim())
			CBF_THROW_RUNTIME_ERROR(m_Name + ": Effector Transform and Resource dimension mismatch: " << m_EffectorTransform->resource_dim() << " is not equal to " << m_Resource->dim());
	}	

	
	#ifdef CBF_HAVE_XSD
		SubordinateController::SubordinateController(const CBFSchema::SubordinateController &xml_instance, ObjectNamespacePtr object_namespace) :
			Controller(xml_instance, object_namespace)
		{
			CBF_DEBUG("Constructing");
			if (xml_instance.Name())
				m_Name = *xml_instance.Name();

			Float coefficient = xml_instance.Coefficient();

			std::vector<ConvergenceCriterionPtr> convergence_criteria;

			for (
				::CBFSchema::SubordinateController::ConvergenceCriterion_const_iterator it = 
					xml_instance.ConvergenceCriterion().begin();
				it != xml_instance.ConvergenceCriterion().end();
				++it
			) {
				CBF_DEBUG("adding criterion");
				ConvergenceCriterionPtr criterion = XMLObjectFactory::instance()->create<ConvergenceCriterion>(*it, object_namespace);
				convergence_criteria.push_back(criterion);
			}


			CBF_DEBUG("Creating reference...");
			ReferencePtr reference = 
				XMLObjectFactory::instance()->create<Reference>(xml_instance.Reference(), object_namespace);

		
			//! Instantiate the potential
			CBF_DEBUG("Creating potential...");
			// m_Potential = XMLObjectFactory<Potential, CBFSchema::Potential>::instance()->create(xml_instance.Potential());
			PotentialPtr potential = 
				XMLObjectFactory::instance()->create<Potential>(xml_instance.Potential(), object_namespace);

      //! Instantiate the taskspace planner
      CBF_DEBUG("Creating task space planner...");
      TaskSpacePlannerPtr planner;
      //TaskSpacePlannerPtr planner =
      //  XMLObjectFactory::instance()->create<TaskSpacePlanner>(xml_instance.TaskSpacePlanner(), object_namespace);

			//! Instantiate the Effector transform
			CBF_DEBUG("Creating sensor transform...");
			SensorTransformPtr sensor_transform = XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.SensorTransform(), object_namespace);
		
			//CBF_DEBUG(m_SensorTransform.get())
		
			//! Instantiate the Effector transform
			CBF_DEBUG("Creating effector transform...");
			EffectorTransformPtr effector_transform = XMLObjectFactory::instance()->create<EffectorTransform>(xml_instance.EffectorTransform(), object_namespace);
		
			CBF_DEBUG("Creating combination strategy...");
			CombinationStrategyPtr combination_strategy  = 
				XMLObjectFactory::instance()->create<CombinationStrategy>(xml_instance.CombinationStrategy(), object_namespace);

			//! Instantiate the subordinate controllers
			CBF_DEBUG("Creating subordinate controller(s)...");
			std::vector<SubordinateControllerPtr> subordinate_controllers;
			for (
				::CBFSchema::SubordinateController::SubordinateController1_const_iterator it = xml_instance.SubordinateController1().begin(); 
				it != xml_instance.SubordinateController1().end();
				++it
			)
			{
				CBF_DEBUG("Creating subordinate controller...");
				CBF_DEBUG("------------------------");
				//! First we see whether we can construct a controller from the xml_document
				SubordinateControllerPtr controller = XMLObjectFactory::instance()->create<SubordinateController>(*it, object_namespace);
 				subordinate_controllers.push_back(controller);
				CBF_DEBUG("------------------------");
			}

			init(
				coefficient,
				convergence_criteria,
				reference,
				potential,
        planner,
				sensor_transform,
				effector_transform,
				subordinate_controllers,
				combination_strategy
			);
		}

		PrimitiveController::PrimitiveController(const CBFSchema::PrimitiveController &xml_instance, ObjectNamespacePtr object_namespace) :
			SubordinateController(xml_instance, object_namespace) {

			ResourcePtr res = XMLObjectFactory::instance()->create<Resource>(xml_instance.Resource(), object_namespace);
			init(res);
		}


		static XMLDerivedFactory<PrimitiveController, ::CBFSchema::PrimitiveController> x;
		static XMLDerivedFactory<SubordinateController, ::CBFSchema::SubordinateController> x2;
		
	#endif

} // namespace 


