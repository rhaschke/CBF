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

#ifndef CBF_KDL_TRANSFORMS_HH
#define CBF_KDL_TRANSFORMS_HH

#include <boost/shared_ptr.hpp>

#include <sstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <cbf/sensor_transform.h>
#include <cbf/namespace.h>
#include <cbf/effector_transform.h>
#include <cbf/quaternion.h>

//! Forward declarations for stuff from KDL namespace that's only
//! used by way of references
namespace KDL {
	class Tree;
	class Chain;
	class JntArray;
	class Frame;
	class Jacobian;
	class ChainJntToJacSolver;
	class TreeJntToJacSolver;
	class ChainFkSolverPos_recursive;
	class ChainFkSolverVel_recursive;
	class TreeFkSolverPos_recursive;
	class TreeFkSolverVel_wdls;
}

//! Forward declarations for XML instance types
// CBF_PLUGIN_PREAMBLE EffectorTransformType;
namespace CBFSchema {
	class KDLChainPoseSensorTransform;
	class KDLChainPositionSensorTransform;
	class KDLChainAxisAngleSensorTransform;
	class KDLChainQuaternionSensorTransform;
	class KDLTreePositionSensorTransform;
	class KDLTreeAxisAngleSensorTransform;

	class ChainBase;
	class TreeBase;
}

namespace CBF {
	
	/**
		@brief Abstract base class for KDL based transform classes.
	
		This class encapsulates the basic funcionality of the different concrete classes
		which implement different representations for the orientation part of the transform 
		(e.g. euler angles, quaternions, screws and twists).
	
		This is an abstract class and thus it cannot be instantiated directly..
	*/
	struct BaseKDLChainSensorTransform : public SensorTransform {
		protected:
			boost::shared_ptr<KDL::Chain> m_Chain;
			boost::shared_ptr<KDL::ChainJntToJacSolver> m_JacSolver;
			boost::shared_ptr<KDL::ChainFkSolverPos_recursive> m_FKSolver;
			boost::shared_ptr<KDL::ChainFkSolverVel_recursive> m_FKVelSolver;
	
			FloatMatrix m_Twists;
	
			//! Intermediate result
			boost::shared_ptr<KDL::Frame> m_Frame;
	
			//! Intermediate result
			boost::shared_ptr<KDL::Jacobian> m_Jacobian;

			unsigned int m_SensorDim;
			unsigned int m_TaskDim;
			unsigned int m_ResourceDim;
	
		public:
			//! constructor, initializes all members
			BaseKDLChainSensorTransform(
				boost::shared_ptr<KDL::Chain> chain
			);
	
			//! This constructor is only implemented when XSD support is enabled
			BaseKDLChainSensorTransform(
				const CBFSchema::ChainBase &xml_chain_instance, 
				const CBFSchema::SensorTransform &xml_st_instance,
				ObjectNamespacePtr object_namespace
			);
	
			//! For all derived types the resource dim is always the same: the number of joints of the kinematic chain
			virtual unsigned int resource_dim() const ;

			//! initialize all members
			virtual void init_solvers();
	
			//! It reads the current resource values and updates the KDL::Jacobian matrix..
			virtual void update(const FloatVector &resource_value);

      /**
        This return the current position.
      */
      void get_position(FloatVector &result);

      /**
        This reads the current orientation.
      */
      void get_quaternion(FloatVector &result);

			boost::shared_ptr<KDL::Chain> chain() { return m_Chain; }
	};
	
	
	
	
	//! SensorTransform of a KDL chain, representing end-effector pose
	struct KDLChainPoseSensorTransform : public BaseKDLChainSensorTransform
	{
		KDLChainPoseSensorTransform(
			const CBFSchema::KDLChainPoseSensorTransform &xml_instance, ObjectNamespacePtr object_namespace
		);

		KDLChainPoseSensorTransform(
			boost::shared_ptr<KDL::Chain> chain
		);

		virtual unsigned int task_dim() const { return 6u; }

		virtual void update(const FloatVector &resource_value);
	};
	typedef boost::shared_ptr<KDLChainPoseSensorTransform> KDLChainPoseSensorTransformPtr;




	//! SensorTransform of a KDL chain, representing end-effector position
	struct KDLChainPositionSensorTransform : public BaseKDLChainSensorTransform
	{
		KDLChainPositionSensorTransform(
			const CBFSchema::KDLChainPositionSensorTransform &xml_instance, ObjectNamespacePtr object_namespace
		);
	
		KDLChainPositionSensorTransform(
			boost::shared_ptr<KDL::Chain> chain
		);

		virtual void update(const FloatVector &resource_value);
	};
	typedef boost::shared_ptr<KDLChainPositionSensorTransform> KDLChainPositionSensorTransformPtr;
	
	typedef boost::shared_ptr<
		KDLChainPositionSensorTransform
	> KDLChainPositionSensorTransformPtr;
	
	
	//! SensorTransform of a KDL chain, representing end-effector orientation as angular velocity vector
	struct KDLChainAxisAngleSensorTransform : public BaseKDLChainSensorTransform
	{
		KDLChainAxisAngleSensorTransform (const CBFSchema::KDLChainAxisAngleSensorTransform &xml_instance, ObjectNamespacePtr object_namespace);
	
		KDLChainAxisAngleSensorTransform(
			boost::shared_ptr<KDL::Chain> chain
		);

		virtual void update(const FloatVector &resource_value);
	};
	
	typedef boost::shared_ptr<KDLChainAxisAngleSensorTransform> KDLChainAxisAngleSensorTransformPtr;	



	/**
		@brief This class implements the SensorTransform for an arbitrary KDL chain. The task space is
		the orientation of the end effector of the chain specified in quaternion representation.
	*/
	struct KDLChainQuaternionSensorTransform : public BaseKDLChainSensorTransform
	{
		KDLChainQuaternionSensorTransform (const CBFSchema::KDLChainQuaternionSensorTransform &xml_instance, ObjectNamespacePtr object_namespace);

		KDLChainQuaternionSensorTransform(
			boost::shared_ptr<KDL::Chain> chain
		);

		virtual void update(const FloatVector &resource_value);
	};

	typedef boost::shared_ptr<KDLChainQuaternionSensorTransform> KDLChainQuaternionSensorTransformPtr;





	/**
		@brief Abstract base class for KDL based transform classes.
	
		This class encapsulates the basic funcionality of the different concrete classes
		which implement different representations for the orientation part of the transform 
		(e.g. euler angles, quaternions, screws and twists).
	
		This is an abstract class and thus it cannot be instantiated directly..
	*/
	struct BaseKDLTreeSensorTransform : public SensorTransform {
		protected:
			unsigned int m_SensorDim;
			unsigned int m_TaskDim;
			unsigned int m_ResourceDim;

			boost::shared_ptr<KDL::Tree> m_Tree;
			boost::shared_ptr<KDL::TreeJntToJacSolver> m_JacSolver;
			boost::shared_ptr<KDL::TreeFkSolverPos_recursive> m_FKSolver;
			boost::shared_ptr<KDL::TreeFkSolverVel_wdls> m_FKVelSolver;
	
			FloatMatrix m_Twists;
	
			//! Intermediate result
			std::vector<boost::shared_ptr<KDL::Frame> > m_Frames;
	
			//! Intermediate result
			std::vector<boost::shared_ptr<KDL::Jacobian> > m_Jacobians;

			//! The segment identifiers for which to solve the FK
			std::vector<std::string> m_SegmentNames;
	
		public:
			/**
				Do not use instances of this class unless the tree has been set with a valid tree
				and init_solvers() was called. This should have been done by subclasses without
				any user intervention.
			*/
			BaseKDLTreeSensorTransform(
				boost::shared_ptr<KDL::Tree> tree, 
				const std::vector<std::string> &segment_names
			);
	
			//! This constructor is only implemented when XSD support is enabled..
			BaseKDLTreeSensorTransform(
				const CBFSchema::TreeBase &xml_tree_instance, 
				const CBFSchema::SensorTransform &xml_st_instance, 
				ObjectNamespacePtr object_namespace
			);
	
			/**
				For all derived types the resource dim is always the same: the number of joints
				of the kinematic tree
			*/
			virtual unsigned int resource_dim() const ;

			/**
				Call this function once m_Tree is set to a valid tree..
			*/
			virtual void init_solvers();
	
			/**
				This reads the current resource values and updates the KDL::Jacobian matrix..
			*/
			virtual void update(const FloatVector &resource_value);

			boost::shared_ptr<KDL::Tree> tree() { return m_Tree; }
	};
	
	


	/**
		@brief This class implements the SensorTransform for an arbitrary KDL tree. The task space is
		the position of the end effector of the tree.
	*/
	struct KDLTreePositionSensorTransform : public BaseKDLTreeSensorTransform
	{
		KDLTreePositionSensorTransform(const CBFSchema::KDLTreePositionSensorTransform &xml_instance, ObjectNamespacePtr object_namespace);
	
		KDLTreePositionSensorTransform(
			boost::shared_ptr<KDL::Tree> tree, 
			std::vector<std::string> segment_names
		);

		virtual void update(const FloatVector &resource_value);
	};
	
	typedef boost::shared_ptr<KDLTreePositionSensorTransform> KDLTreePositionSensorTransformPtr;
	
	

	/**
		@brief This class implements the SensorTransform for an arbitrary KDL tree. The task space is
		the orientation of the end effector of the tree specified in compact axis-angle representation
		(compact meaning here that the angle is absorbed into the length of the axid vector resulting
		in a 3D representation.
	*/
	struct KDLTreeAxisAngleSensorTransform : public BaseKDLTreeSensorTransform
	{
		KDLTreeAxisAngleSensorTransform(
			const CBFSchema::KDLTreeAxisAngleSensorTransform &xml_instance, ObjectNamespacePtr object_namespace
		);
	
		KDLTreeAxisAngleSensorTransform(
			boost::shared_ptr<KDL::Tree> tree, 
			std::vector<std::string> segment_names
		);

		virtual void update(const FloatVector &resource_value);
	};
	
	typedef boost::shared_ptr<
		KDLTreeAxisAngleSensorTransform
	> KDLTreeAxisAngleSensorTransformPtr;	


} // namespace

#endif
