// GstUtil.cpp: Generalized Gstreamer utilities for pipeline configuration.
//
//   Copyright (C) 2008, 2009, 2010, 2011, 2012
//   Free Software Foundation, Inc.
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


#ifdef HAVE_CONFIG_H
#include "gnashconfig.h"
#endif

#include <sstream>
#include <vector>

#include "GstUtil.h"
#include "log.h"
#include "GnashException.h"

#include "swfdec_codec_gst.h"

#ifdef HAVE_GST_PBUTILS_INSTALL_PLUGINS_H
#include <gst/pbutils/pbutils.h>
#include <gst/pbutils/missing-plugins.h>
#include <gst/pbutils/install-plugins.h>
#endif // HAVE_GST_PBUTILS_INSTALL_PLUGINS_H


namespace gnash {
namespace media {
namespace gst {

GstElement* GstUtil::get_audiosink_element()
{   
    //MUST be static to get a numbered name for each non-trivial pipeline created 
    static int numGnashRcSinks = 0;
    
    /*These MAY be static for CPU optimization
     *But, the memory cost at global scope is probably
     *worse overall than the CPU cost at initialization time.*/
    const std::string GNASHRCSINK = "gnashrcsink";
    const std::string sAudioSink =
      RcInitFile::getDefaultInstance().getGstAudioSink();
    
    //Can't be static. One of these must be created for each call
    GstElement* element;
    
    if(sAudioSink.find('!') != std::string::npos) //Found a non-trivial pipeline - bin it
    {
        element = gst_parse_bin_from_description(sAudioSink.c_str(), true, nullptr);
        if(element != nullptr)
        {
           std::ostringstream o;
           o << numGnashRcSinks++;
           gst_element_set_name(element, (GNASHRCSINK + o.str()).c_str());
        }
    }
    else //Found a trivial pipeline that doesn't need a bin
    {
        element = gst_element_factory_make(sAudioSink.c_str(), nullptr);
    }
    
    if(!element)
    {
        log_debug(_("Unable to retrieve a valid audio sink from ~/.gnashrc"));
        
        element = gst_element_factory_make("autoaudiosink", nullptr);
        
        if(!element)
        {
            log_debug(_("Unable to retrieve a valid audio sink from autoaudiosink"));
            
            element = gst_element_factory_make("gconfaudiosink", nullptr);
            
            if(!element)
                log_error(_("Unable to retrieve a valid audio sink from gconfaudiosink\n%s"),
                        _("Sink search exhausted: you won't be able to hear sound!"));
        }
    }
    
    if(element)
    {
        log_debug(_("Got a non-NULL audio sink; its wrapper name is: %s"), _(GST_ELEMENT_NAME(element)));
    }
    
    return element;
}

/**
     This function is not thread-safe.
**/
bool
GstUtil::check_missing_plugins(GstCaps* caps, HostInterface* eventHandler)
{
    GstElementFactory * factory = swfdec_gst_get_element_factory(caps);

    if (factory) {
        gst_object_unref(factory);
        return true;
    }

#ifdef HAVE_GST_PBUTILS_INSTALL_PLUGINS_H

    if(!eventHandler){
	throw new GnashException("GstUtil::check_missing_plugins was not given a HostInterface");
    }

    if (GstUtil::no_plugin_install){
        return false;
    }

    gst_pb_utils_init();


    if (!gst_install_plugins_supported()) {
        log_error(_("Missing plugin, but plugin installing not supported."
                    " Will try anyway, but expect failure."));
    }

    char* detail = gst_missing_decoder_installer_detail_new(caps);
    if (!detail) {
        log_error(_("Missing plugin, but failed to convert it to gst"
                    " missing plugin detail."));
        return false;
    }

    char* details[] =  { detail, nullptr };

    std::vector<void*> * callback_data = new std::vector<void*>;
    callback_data->push_back(detail);
    callback_data->push_back(eventHandler);

    GstInstallPluginsReturn ret = gst_install_plugins_async(details, nullptr, GstUtil::plugin_installer_return, callback_data);

    if (ret == GST_INSTALL_PLUGINS_STARTED_OK) {
	eventHandler->call(HostMessage(HostMessage::EXTERNALINTERFACE_STOPPLAY));
	return true;
    } else {
	log_error(_("gst_install_plugins_async failed. Please report."));
	g_free(detail);
	return false;
    }

#else
    log_error(_("Missing plugin, but automatic plugin installation not "
                "available."));
    return false;
#endif  // end of HAVE_GST_PBUTILS_INSTALL_PLUGINS_H

}

#ifdef HAVE_GST_PBUTILS_INSTALL_PLUGINS_H

// static
bool GstUtil::no_plugin_install = false;

// Helper function for plugin_installer_return below.
// Called when all or some plugins were successfully installed
// static
void
GstUtil::plugin_success_dialog(const char* success_msg,
			       const char* fail_msg,
			       HostInterface* eventHandler)
{
    if (! gst_update_registry()) {
        log_error(_("gst_update_registry failed. You'll need to "
		    "restart Gnash to use the new plugins."));
	eventHandler->call(HostMessage(HostMessage::NOTIFY_ERROR,
				      std::string(fail_msg)));
	GstUtil::no_plugin_install = true;
    } else {
      bool restart = boost::any_cast<bool>(eventHandler->call(HostMessage(HostMessage::QUERY,
									 std::string(success_msg))));
	if(restart){
	    eventHandler->call(HostMessage(HostMessage::EXTERNALINTERFACE_REWIND));
	}
    }
}

// Helper function for plugin_installer_return below.
// Called when there is an error during plugin installation
// static
void
GstUtil::plugin_fail_dialog(const char* fail_msg,
			    HostInterface* eventHandler)
{
    GstUtil::no_plugin_install = boost::any_cast<bool>(eventHandler->call(HostMessage(HostMessage::QUERY,
										      std::string(fail_msg)
										      + std::string(_(" Do you wish to block any further attempts at plugin installation?")))));
}



// Callback function for use by the plugin installer
void
GstUtil::plugin_installer_return(GstInstallPluginsReturn result,
				 gpointer user_data)
{
    std::vector<void*> * v((std::vector<void*> *) (user_data));
    HostInterface* eventHandler = (HostInterface*) (v->at(1));
    char* detail = (char*) v->at(0);

    g_free(detail);

    if(!eventHandler){
      throw new GnashException("plugin_installer_return was not given a HostInterface");
    }
    switch(result) {
    case GST_INSTALL_PLUGINS_SUCCESS:
	plugin_success_dialog(_("Plugin installation succeeded. Do you wish to restart the movie from the beginning?"),_("Plugin installation succeeded, but could not be registered by Gnash. You will need to restart Gnash to see all content in this file."), eventHandler);
	break;
    case GST_INSTALL_PLUGINS_NOT_FOUND:
        plugin_fail_dialog(_("A plugin needed for playing this file was not found. You may still play the file, but some content may not be shown."), eventHandler);
	break;
    case GST_INSTALL_PLUGINS_ERROR:
        plugin_fail_dialog(_("There was an error during installation of a plugin needed for playing this file. You may still play the file, but some content may not be shown."), eventHandler);
	break;
    case GST_INSTALL_PLUGINS_PARTIAL_SUCCESS:
        plugin_fail_dialog(_("Only some of the plugins needed to see this movie could be installed. To see all content here you will need to install these."), eventHandler);
	plugin_success_dialog(_("Do you wish to restart the movie from the beginning?"),_("Some plugins were installed, but could not be registered by Gnash. You will need to restart gnash to see more content in this file. To see all content in this file you also need to install the remaining plugins"), eventHandler);
	break;
    case GST_INSTALL_PLUGINS_USER_ABORT:
        plugin_fail_dialog("", eventHandler);
	break;
    default:
	throw new GnashException("Gst plugin installer returned unrecognized error code.");
    }
    eventHandler->call(HostMessage(HostMessage::EXTERNALINTERFACE_PLAY));
    g_free(v);
    return;
}

#endif  // end of HAVE_GST_PBUTILS_INSTALL_PLUGINS_H

} // gnash.media.gst namespace
} // gnash.media namespace 
} // namespace gnash

// Local Variables:
// mode: C++
// End:

