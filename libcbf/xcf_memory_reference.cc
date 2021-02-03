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


    Copyright 2010 Viktor Richter
*/

#include <cbf/xcf_memory_reference.h>
#include <cbf/utilities.h>

#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>
#include <cbf/foreign_object.h>

#include <boost/bind.hpp>

namespace CBF {

	namespace mi = memory::interface;

	#ifdef CBF_HAVE_XSD
		XCFMemoryReference::XCFMemoryReference(const CBFSchema::XCFMemoryReference &xml_instance, 
															ObjectNamespacePtr object_namespace)
			:
			m_MemoryInterface(mi::MemoryInterface::getInstance(xml_instance.URI())),
			m_ReferenceName(xml_instance.ReferenceName()),
			m_Dim(xml_instance.Dimension())
		{
			init();
		}

		static XMLDerivedFactory<XCFMemoryReference, CBFSchema::XCFMemoryReference> x;
	#endif

	XCFMemoryReference::XCFMemoryReference
		(const std::string &uri, const std::string &reference_name, unsigned int dim) 
		: 
		m_MemoryInterface(mi::MemoryInterface::getInstance(uri)),
		m_ReferenceName(reference_name),
		m_Dim(dim)
	{ 	
		init();
	}

	void XCFMemoryReference::init() {
		// Creating an XCFMemoryReferenceInfo xml and inserting at XCFMemory.
		// This way the Dimension will be published.
		CBF_DEBUG("creating info document");

		CBFSchema::XCFMemoryReferenceInfo v(m_ReferenceName, m_Dim);

		std::ostringstream s;
		CBFSchema::XCFMemoryReferenceInfo_ (s, v);
		std::string document = m_MemoryInterface -> insert(s.str());

		CBF_DEBUG("Document" << document);

		mi::Condition condition((mi::Event::REPLACE | mi::Event::INSERT), XPathString());

		boost::function<void (const mi::Event&) > f;

		f = boost::bind(
			boost::mem_fn(&XCFMemoryReference::set_reference), 
			this,
			_1
		);


		mi::TriggeredAction triggeredAction(f);

		m_MemoryInterface -> add(condition, triggeredAction);
		CBF_DEBUG("XCFMemoryReference initialized");
	}

	void XCFMemoryReference::update()  {
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ReferenceMonitor); 
		if (m_TempReference.size() == m_Dim) {
			m_References.resize(1);
			m_References[0] = m_TempReference;
		}
	}

	void XCFMemoryReference::set_reference(const memory::interface::Event &event) {
		CBF_DEBUG("in");
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(m_ReferenceMonitor); 
		CBF_DEBUG("locked");

		CBF_DEBUG("doc: " << event.getDocument());

		std::string documentText = event.getDocument().getRootLocation().getDocumentText();

		std::istringstream s(documentText);
		std::unique_ptr<CBFSchema::XCFMemoryReferenceVector> reference = 
				CBFSchema::XCFMemoryReferenceVector_(s, xml_schema::flags::dont_validate);

		CBF_DEBUG("create vector");
		m_TempReference = 
			*XMLObjectFactory::instance()->create<ForeignObject<FloatVector> >(
				reference -> Vector(), ObjectNamespacePtr(new ObjectNamespace())
			)->m_Object;

		CBF_DEBUG("vector: " << m_TempReference);

		CBF_DEBUG("vector created");
		if (m_TempReference.size() != dim()) {
			CBF_DEBUG("meeeh!!!");
			CBF_THROW_RUNTIME_ERROR("Dimensions of xml vector not matching the dimension of this reference");
		}
	}
} // namespace

