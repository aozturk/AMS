#pragma once

#include <fstream>
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/NamedNodeMap.h"

namespace AMS {
    static std::vector<std::string> parse()
    {
        std::vector<std::string> peers;

        std::ifstream in("../config.xml");
        if (in.good()) {
            IService::instance().logger().information("config.xml file read successfully.");

            Poco::XML::InputSource src(in);
            Poco::XML::DOMParser parser;
            Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parse(&src);
            Poco::XML::NodeIterator it(pDoc, Poco::XML::NodeFilter::SHOW_ALL);
            Poco::XML::Node* pNode = it.nextNode();
            while (pNode)
            {
                if (pNode->nodeName() == "peer") {         
                    Poco::XML::Node* pChild = pNode->firstChild();
                    while (pChild) {
                        peers.push_back(pChild->nodeValue());
                        pChild = pChild->nextSibling();
                    }
                }

                //Poco::XML::NamedNodeMap* nodeMap = pNode->attributes();
                //if (nodeMap) {
                //    for (int i=0; i < nodeMap->length(); ++i) {
                //        Poco::XML::Node* pAttr = nodeMap->item(i);
                //        std::cout<<pAttr->nodeName()<<":"<< pAttr->nodeValue()<<std::endl;
                //    }
                //}

                pNode = it.nextNode();
            }
        } else {
            IService::instance().logger().warning("config.xml file does not exist!");
        }
        return peers;
    }
}