// 
//   Copyright (C) 2005, 2006 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// Linking Gnash statically or dynamically with other modules is making a
// combined work based on Gnash. Thus, the terms and conditions of the GNU
// General Public License cover the whole combination.
//
// As a special exception, the copyright holders of Gnash give you
// permission to combine Gnash with free software programs or libraries
// that are released under the GNU LGPL and with code included in any
// release of Talkback distributed by the Mozilla Foundation. You may
// copy and distribute such a system following the terms of the GNU GPL
// for all but the LGPL-covered parts and Talkback, and following the
// LGPL for the LGPL-covered parts.
//
// Note that people who make modified versions of Gnash are not obligated
// to grant this special exception for their modified versions; it is their
// choice whether to do so. The GNU General Public License gives permission
// to release a modified version without this exception; this exception
// also makes it possible to release a modified version which carries
// forward this exception.
// 
//
//

#ifndef __XML_H__
#define __XML_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//#define DEBUG_MEMORY_ALLOCATION 1
#include <vector>
#include "log.h"
#include "action.h"
#include "impl.h"

#ifdef HAVE_LIBXML

#include "xmlattrs.h"
#include "xmlnode.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>

namespace gnash {
  
/// XML class
class XML {
public:
    XML();
    XML(tu_string xml_in);
    XML(struct node * childNode);
    virtual ~XML();
  
    // Methods
    // This is the base method used by both parseXML() and load().
    bool parseDoc(xmlDocPtr document, bool mem);
    // Parses an XML document into the specified XML object tree.
    bool parseXML(tu_string xml_in);
    // Loads a document (specified by
    bool load(const char *filespec);
    // the XML object) from a URL.

    // An event handler that returns a
    bool onLoad();
    // Boolean value indicating whether
    // the XML object was successfully
    // loaded with XML.load() or
    // XML.sendAndLoad().

    // Appends a node to the end of the specified object's child list.
    void appendChild(XMLNode *node);
    
    virtual bool on_event(gnash::event_id id);
    virtual void	on_event_load();
    bool loaded()    { return _loaded; }
    
    XMLNode *firstChild() {
        return _nodes;
        //return _node_data[0];
    }
    
    void clear() {
        delete _nodes;
    }
  
  std::vector<XMLNode *> childNodes();
  
    const char *stringify(XMLNode *xml);
    //  Returns true if the specified node has child nodes; otherwise, returns false.
  bool hasChildNodes() {
    if (_nodes) {
      if (_nodes->_children.size()) {
        return true;
      } 
    }
    return false;
  }
    
    XMLNode *extractNode(xmlNodePtr node, bool mem);
    XMLNode *processNode(xmlTextReaderPtr reader, XMLNode *node);

    void  change_stack_frame(int frame, gnash::as_object *xml, gnash::as_environment *env);
//    void  setupStackFrames(gnash::as_object *xml, gnash::as_environment *env);
    void  cleanupStackFrames( XMLNode *data);
    as_object *setupFrame(gnash::as_object *xml, XMLNode *data, bool src);
  
    const char *nodeNameGet()    { return _nodename; }
    const char *nodeName();
    const char *nodeValue();
    void nodeNameSet(char *name);
    void nodeValueSet(char *value);
    int length()                 { return _nodes->length(); }
  
    // These 6 have to 
    void addRequestHeader(const char *name, const char *value);
    XMLNode &cloneNode(XMLNode &newnode, bool deep);
    XMLNode *createElement(const char *name);
    XMLNode *createTextNode(const char *name);
    void insertBefore(XMLNode *newnode, XMLNode *node);

    void load();
    void parseXML();
    void removeNode();
    void send();
    void sendAndLoad();
    const char *toString();

    int getBytesLoaded()         { return _bytes_loaded; };
    int getBytesTotal()          { return _bytes_total; };

    virtual void	on_xml_event(gnash::event_id id) { on_event(id); }
  
    // Special event handler; 
    void	on_event_close() { on_event(gnash::event_id::SOCK_CLOSE); }
  
    XMLNode *operator [] (int x);
#if 0
    XMLNode *operator = (XMLNode &node) {
        gnash::log_msg("%s: copy element %s\n", __PRETTY_FUNCTION__, node._name);
	//        _nodes = node.;
    }

#endif
    XML *operator = (XMLNode *node) {
        _nodes = node;    
        return this;
    }
    
private:
    xmlDocPtr _doc;
    xmlNodePtr _firstChild;
    
    // Properties
    bool _loaded;
    const char  *_nodename;
    XMLNode     *_nodes;

    int         _bytes_loaded;
    int         _bytes_total;
    
    bool        _contentType;
    bool        _attributes;
    bool        _childNodes;
    bool        _xmlDecl;
    bool        _docTypeDecl;
    bool        _ignoreWhite;
    bool        _lastChild;
    bool        _nextSibling;
    bool        _nodeType;
    bool        _nodeValue;
    bool        _parentNode;
    bool        _status;
    bool        _previousSibling;

};

/// XML ActionScript object
class xml_as_object : public gnash::as_object
{
public:
    XML obj;
#ifdef DEBUG_MEMORY_ALLOCATION
    xml_as_object() {
        log_msg("\tCreating xml_as_object at %p\n", this);
    };
    ~xml_as_object() {
        log_msg("\tDeleting xml_as_object at %p\n", this);
    };
#endif

    virtual bool get_member(const tu_stringi& name, as_value* val) {
        //printf("GET XML MEMBER: %s at %p for object %p\n", name.c_str(), val, this);
        
        if ((name == "firstChild") || (name == "childNodes")) {
//             printf("Returning a self reference for %s for object at %p\n",
//                    name.c_str(), this);
            val->set_as_object(this);
            return true;
        }
        
#if 0
        printf("%s(%s:%d): ERROR: as_member::get() unimplemented!",
               __PRETTY_FUNCTION__, __FILE__, __LINE__);
#else
        as_member m;
        if (m_members.get(name, &m) == false) {
            if (m_prototype != NULL) {
                return m_prototype->get_member(name, val);
            }
            return false;
        } else {
            *val=m.get_member_value();
            return true;
        }
#endif
        return true;
    }
};


void xml_load(const fn_call& fn);
void xml_set_current(const fn_call& fn);
void xml_new(const fn_call& fn);

void xml_addrequestheader(const fn_call& fn);
void xml_appendchild(const fn_call& fn);
void xml_clonenode(const fn_call& fn);
void xml_createelement(const fn_call& fn);
void xml_createtextnode(const fn_call& fn);
void xml_getbytesloaded(const fn_call& fn);
void xml_getbytestotal(const fn_call& fn);
void xml_haschildnodes(const fn_call& fn);
void xml_insertbefore(const fn_call& fn);
void xml_parsexml(const fn_call& fn);
void xml_removenode(const fn_call& fn);
void xml_send(const fn_call& fn);
void xml_sendandload(const fn_call& fn);
void xml_tostring(const fn_call& fn);

// These are the event handlers called for this object
void xml_onload(const fn_call& fn);
void xml_ondata(const fn_call& fn);
void xml_loaded(const fn_call& fn);

// Properties
void xml_nodename(const fn_call& fn);
void xml_nodevalue(const fn_call& fn);


int memadjust(int x);


}	// end namespace gnash


#endif // HAVE_LIBXML

#endif	// __XML_H__


// Local Variables:
// mode: C++
// indent-tabs-mode: t
// End:
