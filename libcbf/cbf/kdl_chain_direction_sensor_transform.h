/*
 * kdl_chain_direction_sensor_transform.h
 *
 * It compute a directional jacobian of any aribtrary vector attached on the end-effector.
 * The default value of the "control direction" is [0 0 1].
 * The "control direction" can be updated by calling "setLocalControlDirection" function at any time.
 * The "control direction" must be expressed by the cordiate system of the end-effector
 *
 *  Created on Mar. 12, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef KDL_CHAIN_DIRECTION_SENSOR_TRANSFORM_H
#define KDL_CHAIN_DIRECTION_SENSOR_TRANSFORM_H

#include <boost/shared_ptr.hpp>

#include <sstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <kdl/frames.hpp>
#include <cbf/sensor_transform.h>
#include <cbf/namespace.h>
#include <cbf/effector_transform.h>
#include <cbf/kdl_transforms.h>

namespace CBF {


class KDLChainDirectionSensorTransform : public BaseKDLChainSensorTransform
{
  public:
    KDLChainDirectionSensorTransform (const KDLChainDirectionSensorTransform &xml_instance, ObjectNamespacePtr object_namespace);

    KDLChainDirectionSensorTransform(
      boost::shared_ptr<KDL::Chain> chain
    );

    virtual unsigned int task_dim() const { return 3u; }

    virtual void update(const FloatVector &resource_value);

    void setLocalControlDirection(const KDL::Vector &control_dir);
  private:
    KDL::Vector mLocalControlDirection;

    KDL::Vector mCurrentControlDirection;
};

typedef boost::shared_ptr<KDLChainDirectionSensorTransform> KDLChainDirectionSensorTransformPtr;

}
#endif // KDL_CHAIN_DIRECTION_SENSOR_TRANSFORM_H