// XMLNode_as.cpp:  ActionScript "XMLNode" class, for Gnash.
//
//   Copyright (C) 2009, 2010, 2011, 2012 Free Software Foundation, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "XMLNode_as.h"

#include <functional>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include "XML_as.h"
#include "VM.h"
#include "log.h"
#include "fn_call.h"
#include "Global_as.h"
#include "NativeFunction.h"
#include "PropertyList.h"
#include "Global_as.h"
#include "Object.h"
#include "Array_as.h"
#include "namedStrings.h"

namespace gnash {

// Function Prototypes
namespace {
    typedef std::pair<std::string, std::string> StringPair;
    typedef std::vector<StringPair> StringPairs;
    void enumerateAttributes(const XMLNode_as& node,
            StringPairs& attributes);
    bool prefixMatches(const StringPairs::value_type& val,
            const std::string& prefix);
    bool namespaceMatches(
            const StringPairs::value_type& val,
            const std::string& ns);    

    as_value xmlnode_new(const fn_call& fn);
    as_value xmlnode_nodeName(const fn_call& fn);
    as_value xmlnode_nodeValue(const fn_call& fn);
    as_value xmlnode_nodeType(const fn_call& fn);
    as_value xmlnode_attributes(const fn_call& fn);
    as_value xmlnode_appendChild(const fn_call& fn);
    as_value xmlnode_cloneNode(const fn_call& fn);
    as_value xmlnode_lastChild(const fn_call& fn);
    as_value xmlnode_firstChild(const fn_call& fn);
    as_value xmlnode_nextSibling(const fn_call& fn);
    as_value xmlnode_childNodes(const fn_call& fn);
    as_value xmlnode_previousSibling(const fn_call& fn);
    as_value xmlnode_parentNode(const fn_call& fn);
    as_value xmlnode_getNamespaceForPrefix(const fn_call& fn);
    as_value xmlnode_getPrefixForNamespace(const fn_call& fn);
    as_value xmlnode_namespaceURI(const fn_call& fn);
    as_value xmlnode_hasChildNodes(const fn_call& fn);
    as_value xmlnode_insertBefore(const fn_call& fn);
    as_value xmlnode_removeNode(const fn_call& fn);
    as_value xmlnode_toString(const fn_call& fn);
    as_value xmlnode_localName(const fn_call& fn);
    as_value xmlnode_prefix(const fn_call& fn);
    void attachXMLNodeInterface(as_object& o);
}

XMLNode_as::XMLNode_as(Global_as& gl)
    :
    _global(gl),
    _object(nullptr),
    _parent(nullptr),
    _attributes(new as_object(gl)),
    _childNodes(nullptr),
    _type(Element)
{
}

XMLNode_as::XMLNode_as(const XMLNode_as& tpl, bool deep)
    :
    _global(tpl._global),
    _object(nullptr),
    _parent(nullptr),
    _attributes(new as_object(_global)),
    _childNodes(nullptr),
    _name(tpl._name),
    _value(tpl._value),
    _type(tpl._type)
{
    // only clone children if in deep mode
    if (deep) {
        const Children& from=tpl._children;
        for (const auto& child : from) {
            XMLNode_as* copy = new XMLNode_as(*child, deep);
            copy->setParent(this);
            _children.push_back(copy);
        }
    }
}

XMLNode_as::~XMLNode_as()
{
    // In practice it is quite likely that the child will be garbage-collected
    // before the parent. See Savannah bug #39404.
    if (_parent ) {
        // NOTE: do not removeChild as it makes too much
        //       noise including calls to string_table
        //       (due to updateChildNodes)
        // See https://savannah.gnu.org/bugs/?40439
        _parent->_children.remove(this);
        _parent = nullptr;
    }

    clearChildren();
}

as_object*
XMLNode_as::object() 
{

    // This is almost the same as if the XMLNode constructor were called,
    // but not quite. There is no __constructor__ property, and when we
    // override _global.XMLNode, we can show that it is not called.
    if (!_object) {
        as_object* o = createObject(_global);
        as_object* xn =
            toObject(getMember(_global, NSV::CLASS_XMLNODE), getVM(_global));
        if (xn) {
            o->set_prototype(getMember(*xn, NSV::PROP_PROTOTYPE));
            o->init_member(NSV::PROP_CONSTRUCTOR, xn);
        }
        o->setRelay(this);
        setObject(o);
    }
    return _object;
}

void
XMLNode_as::updateChildNodes()
{
    if (!_childNodes) return;

    // Clear array of all elements.
    _childNodes->set_member(NSV::PROP_LENGTH, 0.0);

    if (_children.empty()) return;

    VM& vm = getVM(_global);

    // Set up the array without calling push()!
    const size_t size = _children.size();
    Children::const_iterator it = _children.begin();
    for (size_t i = 0; i != size; ++i, ++it) {
        XMLNode_as* node = *it;
        const ObjectURI& key = arrayKey(vm, i);
        _childNodes->set_member(key, node->object());

        // All elements are set to readonly.
        _childNodes->set_member_flags(key, PropFlags::readOnly);
    }
}

as_object*
XMLNode_as::childNodes()
{
    if (!_childNodes) {
        _childNodes = _global.createArray();
        updateChildNodes();
    }
    return _childNodes;
}

bool
XMLNode_as::hasChildNodes() const
{
    return !_children.empty();
}

XMLNode_as*
XMLNode_as::firstChild() const
{
    if (_children.empty()) return nullptr;
    return _children.front();
}

XMLNode_as*
XMLNode_as::cloneNode(bool deep) const
{
    XMLNode_as* newnode = new XMLNode_as(*this, deep);
    return newnode;
}

XMLNode_as*
XMLNode_as::lastChild() const
{
	if (_children.empty()) {
        return nullptr;
	}
	return _children.back();
}

void
XMLNode_as::removeChild(XMLNode_as* node)
{
    node->setParent(nullptr);
    _children.remove(node);
    updateChildNodes();
}

void
XMLNode_as::appendChild(XMLNode_as* node)
{
    //assert(node);
    node->setParent(this);
    _children.push_back(node);
    updateChildNodes();
}

bool
XMLNode_as::descendsFrom(XMLNode_as* node) const
{
    if (node == this) {
        return true;
    }
    XMLNode_as* parent = getParent();
    if (parent) {
        return parent->descendsFrom(node);
    }

    return false;
}

void
XMLNode_as::insertBefore(XMLNode_as* newnode, XMLNode_as* pos)
{
    //assert(_object);

	// find iterator for positional parameter
    Children::iterator it = std::find(_children.begin(), _children.end(), pos);
    if (it == _children.end()) {
        IF_VERBOSE_ASCODING_ERRORS(
        log_aserror(_("XMLNode.insertBefore(): positional parameter "
                "is not a child of this node"));
        );
        return;
    }

    _children.insert(it, newnode);

    XMLNode_as* parent = newnode->getParent();
    if (parent) {
        parent->removeChild(newnode);
    }
    
    newnode->setParent(this);
    updateChildNodes();
}

XMLNode_as*
XMLNode_as::previousSibling() const
{
    if (!_parent) return nullptr;
 	if (_parent->_children.size() <= 1) return nullptr;

    XMLNode_as *previous_node = nullptr;
    for (XMLNode_as * child : _parent->_children) {

        if (child == this) return previous_node;
		
        previous_node = child;
    }

    return nullptr;
}

XMLNode_as*
XMLNode_as::nextSibling() const
{

    if (!_parent) return nullptr;

    if (_parent->_children.size() <= 1) return nullptr;

    XMLNode_as *previous_node = nullptr;
    for (Children::reverse_iterator itx = _parent->_children.rbegin();
            itx != _parent->_children.rend(); ++itx) {

        if (*itx == this) return previous_node;
		previous_node = *itx;
    }

    return nullptr;
}

void
XMLNode_as::toString(std::ostream& xmlout, bool encode) const
{
    stringify(*this, xmlout, encode);
}

void
XMLNode_as::setAttribute(const std::string& name, const std::string& value)
{
    if (_attributes) {
        VM& vm = getVM(_global);
        _attributes->set_member(getURI(vm, name), value);
    }
}

bool
XMLNode_as::getPrefixForNamespace(const std::string& ns, std::string& prefix)
    const
{
    const XMLNode_as* node = this;
    StringPairs::const_iterator it; 
    StringPairs attrs;
    
    while (node) {
        enumerateAttributes(*node, attrs);
        if (!attrs.empty())
        {
            it = std::find_if(attrs.begin(), attrs.end(), 
                        std::bind(namespaceMatches, std::placeholders::_1, ns));
            if (it != attrs.end()) break;
        }
        node = node->getParent();
    }

    // None found.
    if (!node) return false;

    // Return the matching prefix
    const std::string& name = it->first;

    if (name.length() == 5) {
        return true;
    }

    assert (name.length() >= 6);
    
    if (name[5] != ':') return false;

    // Can also be empty.
    prefix = name.substr(6);
    return true;    
}

void
XMLNode_as::getNamespaceForPrefix(const std::string& prefix, std::string& ns)
    const
{
    const XMLNode_as* node = this;
    StringPairs::const_iterator it; 
    StringPairs attrs;
    
    while (node) {

        enumerateAttributes(*node, attrs);

        if (!attrs.empty()) {

            it = std::find_if(attrs.begin(), attrs.end(), 
                        std::bind(prefixMatches, std::placeholders::_1, prefix));
            if (it != attrs.end()) break;
        }
        node = node->getParent();
    }

    // None found; return undefined
    if (!node) return;

    // Return the matching namespace
    ns = it->second;

}

bool
XMLNode_as::extractPrefix(std::string& prefix) const
{
    prefix.clear();
    if (_name.empty()) return false;

    std::string::size_type pos = _name.find(':');
    if (pos == std::string::npos || pos == _name.size() - 1) {
        return false;
    }

    prefix = _name.substr(0, pos);
    return true;
}

void
XMLNode_as::clearChildren()
{
    for (XMLNode_as* node : _children) {

        node->setParent(nullptr);
        if (!node->_object) {
            // The node is not GC'd because it has no associated object. 
            // See XMLNode_as class docs.
            delete node;
        }
    }
    _children.clear();

    // Reset so that it is reinitialized on next access.
    _childNodes = nullptr;
}

void
XMLNode_as::stringify(const XMLNode_as& xml, std::ostream& xmlout, bool encode) 
{

    const std::string& nodeValue = xml.nodeValue();
    const std::string& nodeName = xml.nodeName();
    NodeType type = xml.nodeType();

#ifdef GNASH_DEBUG
    log_debug("Stringifying node %p with name %s, as_value %s, %u "
	      "attributes and %u children", (void*)&xml, nodeName,
            nodeValue, xml._attributes.size(), xml._children.size());
#endif

    if (!nodeName.empty() || type == Element) {

        xmlout << "<" << nodeName;

        // Process the attributes, if any
        StringPairs attrs;
        enumerateAttributes(xml, attrs);
        if (!attrs.empty()) {

            for (auto& attr : attrs) { 
                escapeXML(attr.second);
                xmlout << " " << attr.first << "=\"" << attr.second << "\"";
            }
        }

        // If the node has no content, just close the tag now
        if (nodeValue.empty() && xml._children.empty()) {
            xmlout << " />";
            return;
        }
        else {
            // Will use a closing tag later
            xmlout << ">";
        }
    }

    // Node as_value first, then children
    if (type == Text)
    {
        Global_as& gl = xml._global;

        // Insert entities.
        std::string escaped(nodeValue);
        escapeXML(escaped);
        const std::string& val = encode ? 
            callMethod(&gl, NSV::PROP_ESCAPE, escaped).to_string() :
            escaped;

	    xmlout << val;
    }

    // Childs, after node as_value.
    for (XMLNode_as* child : xml._children) {

        child->toString(xmlout, encode);
    }

    if (!nodeName.empty() || type == Element) {
        xmlout << "</" << nodeName << ">";
    }
}

void
XMLNode_as::setReachable() 
{
    // If there is a parent, make sure its object is reachable. This goes
    // up towards the root node of tree without marking the XMLNode
    // resources (which would cause infinite recursion).
    if (_parent && _parent->_object) _parent->_object->setReachable();

	// Mark children
    std::for_each(_children.begin(), _children.end(),
            std::mem_fn(&XMLNode_as::setReachable));

	// Mark attributes object
	if (_attributes) _attributes->setReachable();

    if (_object) _object->setReachable();

    if (_childNodes) _childNodes->setReachable();
}

void
registerXMLNodeNative(as_object& where)
{
    VM& vm = getVM(where);
    vm.registerNative(xmlnode_cloneNode, 253, 1);
    vm.registerNative(xmlnode_removeNode, 253, 2);
    vm.registerNative(xmlnode_insertBefore, 253, 3);
    vm.registerNative(xmlnode_appendChild, 253, 4);
    vm.registerNative(xmlnode_hasChildNodes, 253, 5);
    vm.registerNative(xmlnode_toString, 253, 6);
    vm.registerNative(xmlnode_getNamespaceForPrefix, 253, 7);
    vm.registerNative(xmlnode_getPrefixForNamespace, 253, 8);
}

void
xmlnode_class_init(as_object& where, const ObjectURI& uri)
{
    Global_as& gl = getGlobal(where);
    as_object* proto = createObject(gl);
    attachXMLNodeInterface(*proto);
    as_object* cl = gl.createClass(&xmlnode_new, proto);

    where.init_member(uri, cl, as_object::DefaultFlags);

}

namespace {

void
attachXMLNodeInterface(as_object& o)
{
    
    VM& vm = getVM(o);

    const int noFlags = 0;
    
    // No prop flags:
    o.init_member("cloneNode", vm.getNative(253, 1), noFlags);
    o.init_member("removeNode", vm.getNative(253, 2), noFlags);
    o.init_member("insertBefore", vm.getNative(253, 3), noFlags);
    o.init_member("appendChild", vm.getNative(253, 4), noFlags);
    o.init_member("hasChildNodes", vm.getNative(253, 5), noFlags);
    o.init_member("toString", vm.getNative(253, 6), noFlags);
    o.init_member("getNamespaceForPrefix", vm.getNative(253, 7), noFlags);
    o.init_member("getPrefixForNamespace", vm.getNative(253, 8), noFlags);

    const int protectedFlags = 0;

    // Just the protected flag:

    o.init_readonly_property("attributes", &xmlnode_attributes, protectedFlags);
    o.init_readonly_property("childNodes", &xmlnode_childNodes, protectedFlags);
    o.init_readonly_property("firstChild", &xmlnode_firstChild, protectedFlags);
    o.init_readonly_property("lastChild", &xmlnode_lastChild, protectedFlags);
    o.init_readonly_property("nextSibling", 
            &xmlnode_nextSibling, protectedFlags);
    o.init_property("nodeName", &xmlnode_nodeName, 
            &xmlnode_nodeName, protectedFlags);
    o.init_readonly_property("nodeType", &xmlnode_nodeType, protectedFlags);
    o.init_property("nodeValue", &xmlnode_nodeValue, 
            &xmlnode_nodeValue, protectedFlags);
    o.init_readonly_property("parentNode", &xmlnode_parentNode, protectedFlags);
    o.init_readonly_property("previousSibling", 
            &xmlnode_previousSibling, protectedFlags);
    o.init_readonly_property("prefix", &xmlnode_prefix, protectedFlags);
    o.init_readonly_property("localName", &xmlnode_localName, protectedFlags);
    o.init_readonly_property("namespaceURI", 
            &xmlnode_namespaceURI, protectedFlags);
}


as_value
xmlnode_new(const fn_call& fn)
{
    
    as_object* obj = ensure<ValidThis>(fn);

    if (!fn.nargs) {
        return as_value();
    }

    std::unique_ptr<XMLNode_as> xml(new XMLNode_as(getGlobal(fn)));
    xml->nodeTypeSet(XMLNode_as::NodeType(toInt(fn.arg(0), getVM(fn))));

    if (fn.nargs > 1) {
        const std::string& str = fn.arg(1).to_string();
        switch (xml->nodeType())
        {
            case XMLNode_as::Element:
                xml->nodeNameSet(str);
                break;
            default:
                xml->nodeValueSet(str);
                break;
        }
    }
    
    // This sets the relay!
    xml->setObject(obj);
    obj->setRelay(xml.release());

    return as_value();
}


as_value
xmlnode_appendChild(const fn_call& fn)
{

	XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);

	if (!fn.nargs) {
		IF_VERBOSE_ASCODING_ERRORS(
            log_aserror(_("XMLNode::appendChild() needs at least one "
                    "argument"));
		);
		return as_value();
	}

	XMLNode_as* node;
    if (!isNativeType(toObject(fn.arg(0), getVM(fn)), node)) {
		IF_VERBOSE_ASCODING_ERRORS(
            log_aserror(_("First argument to XMLNode::appendChild() is not "
                    "an XMLNode"));
		);
		return as_value();
	}

    if (ptr->descendsFrom(node)) {
        IF_VERBOSE_ASCODING_ERRORS(
        log_aserror(_("XMLNode.appendChild(): attempted to move a node to "
            "among its own descendants."));
        );
        return as_value();
    }

    XMLNode_as* parent = node->getParent();
    if (parent) {
        parent->removeChild(node);
    }
	ptr->appendChild(node);

	return as_value(); 

}

as_value
xmlnode_cloneNode(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);

    bool deep = false;
    if (fn.nargs > 0) deep = toBool(fn.arg(0), getVM(fn));

    as_object* newnode = ptr->cloneNode(deep)->object();
    return as_value(newnode);
}


as_value
xmlnode_insertBefore(const fn_call& fn)
{
	XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);

	if ( fn.nargs < 2 )
	{
		IF_VERBOSE_ASCODING_ERRORS(
		std::stringstream ss; fn.dump_args(ss);
		log_aserror(_("XMLNode.insertBefore(%s) needs at least two "
                "arguments"), ss.str());
		);
		return as_value();
	}

	XMLNode_as* newnode;

    if (!isNativeType(toObject(fn.arg(0), getVM(fn)), newnode)) {
		IF_VERBOSE_ASCODING_ERRORS(
		std::stringstream ss; fn.dump_args(ss);
		log_aserror(_("First argument to XMLNode.insertBefore(%s) is not "
                "an XMLNode"), ss.str());
		);
		return as_value();
	}

	XMLNode_as* pos;

    if (!isNativeType(toObject(fn.arg(1), getVM(fn)), pos)) {
		IF_VERBOSE_ASCODING_ERRORS(
        std::stringstream ss; fn.dump_args(ss);
		log_aserror(_("Second argument to XMLNode.insertBefore(%s) is not "
                "an XMLNode"), ss.str());
		);
		return as_value();
	}

    if (pos->descendsFrom(newnode)) {
        IF_VERBOSE_ASCODING_ERRORS(
        log_aserror(_("XMLNode.insertBefore(): attempted to move a node to "
            "among its own descendants."));
        );
        return as_value();
    }

    ptr->insertBefore(newnode, pos);
    return as_value();
    
}


as_value
xmlnode_getNamespaceForPrefix(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    if (!fn.nargs) {
        return as_value();
    }

    std::string ns;

    ptr->getNamespaceForPrefix(fn.arg(0).to_string(), ns);
    if (ns.empty()) return as_value();
    return as_value(ns);
}


as_value
xmlnode_getPrefixForNamespace(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    if (!fn.nargs) {
        return as_value();
    }

    std::string prefix;

    // Return undefined if none found; otherwise the prefix string found.
    // This can be empty if it is a standard namespace.
    if (!ptr->getPrefixForNamespace(fn.arg(0).to_string(), prefix)) {
        return as_value();
    }
    return as_value(prefix);
}

/// If the node has a prefix, return the matching namespace. Otherwise, 
/// returns a namespaceURI set with the xmlns attribute, searching upwards
/// through parent nodes if necessary.
//
/// This standard namespace can only  be set during XML parsing and cannot
/// be changed or set using attributes.
//
/// Conversely, the similar getNamespaceForPrefix("") can be set and changed
/// through attributes.
as_value
xmlnode_namespaceURI(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);

    // Read-only property
    
    const std::string& name = ptr->nodeName();

    if (name.empty()) {
        as_value null;
        null.set_null();
        return null;
    }

    std::string prefix;
    if (ptr->extractPrefix(prefix)) {
        std::string ns;
        ptr->getNamespaceForPrefix(prefix, ns);
        return as_value(ns);
    }

    // Search recursively for a namespace. Return an empty string
    // if none found.
    XMLNode_as* node = ptr;
    while (node && node->getNamespaceURI().empty()) {
        node = node->getParent();
    }
    if (!node) return as_value("");

    return as_value(node->getNamespaceURI());
}


// Return the prefix part of the node name. If there is no colon, or one
// colon at the end of the string, this is empty. Otherwise it is the part
// up to the first colon.
as_value
xmlnode_prefix(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);

    // Read-only property
    
    if (ptr->nodeName().empty()) {
        as_value null;
        null.set_null();
        return null;
    }
    
    std::string prefix;
    if (!ptr->extractPrefix(prefix)) return as_value("");
    return as_value(prefix);
}


// The local part of a node name. If there is no colon or a single colon
// at the end of the string, this is the whole string. Otherwise all of the
// string after the first colon.
as_value
xmlnode_localName(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);

    // Read-only property
    
    if (ptr->nodeName().empty()) {
        as_value null;
        null.set_null();
        return null;
    }

    const std::string& nodeName = ptr->nodeName();
    if (nodeName.empty()) return as_value("");

    std::string::size_type pos = nodeName.find(':');
    if (pos == std::string::npos || pos == nodeName.size() - 1) {
        return as_value(nodeName);
    }

    return as_value(nodeName.substr(pos + 1));
}


as_value
xmlnode_removeNode(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    
    XMLNode_as* parent = ptr->getParent();
    if (parent) parent->removeChild(ptr);
    return as_value();
}


as_value
xmlnode_toString(const fn_call& fn)
{
    
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    
    std::stringstream ss;
    ptr->toString(ss);

    return as_value(ss.str());
}


as_value
xmlnode_hasChildNodes(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    return as_value(ptr->hasChildNodes());
}


as_value
xmlnode_nodeValue(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    as_value rv;
    rv.set_null();
    
    if ( fn.nargs == 0 )
    {
        const std::string& val = ptr->nodeValue();
        if ( ! val.empty() ) rv = val;
    }
    else
    {
        ptr->nodeValueSet(fn.arg(0).to_string());
    }
    return rv;
}


as_value
xmlnode_nodeName(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    as_value rv;
    rv.set_null();

    if (!fn.nargs) {
        const std::string& val = ptr->nodeName();
        if ( ! val.empty() ) rv = val;
    }
    else {
        ptr->nodeNameSet(fn.arg(0).to_string());
    }
    return rv;
}


as_value
xmlnode_nodeType(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    return as_value(ptr->nodeType());
}


as_value
xmlnode_attributes(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);

    as_object* attrs = ptr->getAttributes();
    if (attrs) return as_value(attrs);
    return as_value(); 
}


/// Read-only property; evaluates the specified XML object and
/// references the first child in the parent node's child
/// list. This property is null if the node does not have
/// children. This property is undefined if the node is a text
/// node. This is a read-only property and cannot be used to
/// manipulate child nodes; use the appendChild(), insertBefore(),
/// and removeNode() methods to manipulate child nodes. 
///
as_value
xmlnode_firstChild(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    as_value rv;
    rv.set_null();

    XMLNode_as* node = ptr->firstChild();
    if (node) {
        rv = node->object();
    }

    return rv;
}


/// Read-only property; an XMLNode as_value that references the last
/// child in the node's child list. The XML.lastChild property
/// is null if the node does not have children. This property cannot
/// be used to manipulate child nodes; use the appendChild(),
/// insertBefore(), and removeNode() methods to manipulate child
/// nodes.
as_value
xmlnode_lastChild(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    as_value rv;
    rv.set_null();

    XMLNode_as* node = ptr->lastChild();
    if (node) {
        rv = node->object();
    }

    return rv;
}


as_value
xmlnode_nextSibling(const fn_call& fn)
{
    as_value rv;
    rv.set_null();

    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    XMLNode_as *node = ptr->nextSibling();
    if (node) {
        rv = node->object();
    }
    return rv;
}


as_value
xmlnode_previousSibling(const fn_call& fn)
{
    as_value rv;
    rv.set_null();

    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    XMLNode_as *node = ptr->previousSibling();
    if (node) {
        rv = node->object();
    }
    return rv;
}


as_value
xmlnode_parentNode(const fn_call& fn)
{
    as_value rv;
    rv.set_null();

    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    XMLNode_as* node = ptr->getParent();
    if (node) {
        rv = node->object();
    }
    return rv;
}

as_value
xmlnode_childNodes(const fn_call& fn)
{
    XMLNode_as* ptr = ensure<ThisIsNative<XMLNode_as> >(fn);
    return ptr->childNodes();
}


void
enumerateAttributes(const XMLNode_as& node, StringPairs& pairs)
{
    pairs.clear();

    as_object* obj = node.getAttributes();
    if (obj) {
        string_table& st = getStringTable(*obj);
        SortedPropertyList attrs = enumerateProperties(*obj);
        for (SortedPropertyList::const_reverse_iterator i = attrs.rbegin(), 
                e = attrs.rend(); i != e; ++i) {
            // TODO: second argument should take version.
            pairs.push_back(
                std::make_pair(i->first.toString(st), i->second.to_string()));
        }
    }

}

/// Return true if this attribute is a namespace specifier and the
/// namespace matches.
bool
namespaceMatches(const StringPairs::value_type& val,
        const std::string& ns)
{
    StringNoCaseEqual noCaseCompare;
    return (noCaseCompare(val.first.substr(0, 5), "xmlns") && 
                noCaseCompare(val.second, ns));
}


bool
prefixMatches(const StringPairs::value_type& val,
        const std::string& prefix)
{
    const std::string& name = val.first;
    StringNoCaseEqual noCaseCompare;

    // An empty prefix searches for a standard namespace specifier.
    // Attributes are stored with no trailing or leading whitespace,
    // so a simple comparison should do. TODO: what about "xmlns:"?
    if (prefix.empty()) {
        return noCaseCompare(name, "xmlns") || noCaseCompare(name, "xmlns:");
    }

    if (!noCaseCompare(name.substr(0, 6), "xmlns:")) return false;

    return noCaseCompare(prefix, name.substr(6));
}

} // anonymous namespace 
} // gnash namespace
// local Variables:
// mode: C++
// indent-tabs-mode: t
// End:

