#include <cbf/qt_sensor_transform.h>
#include <cbf/xml_factory.h>

namespace CBF {

	#ifdef CBF_HAVE_XSD

		QtSensorTransform::QtSensorTransform(const CBFSchema::QtSensorTransform &xml_instance) :
			SensorTransform(xml_instance)
		{
			init(XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.Operand()));
		}

		static XMLDerivedFactory<QtSensorTransform, CBFSchema::QtSensorTransform> x1;
	#endif
}
