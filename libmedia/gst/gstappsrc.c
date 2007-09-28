/* GStreamer
 * Copyright (C) 2007 David Schleef <ds@schleef.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/* Id: */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/base/gstpushsrc.h>

#include <string.h>

#include "gstappsrc.h"


GST_DEBUG_CATEGORY (app_src_debug);
#define GST_CAT_DEFAULT app_src_debug

static const GstElementDetails app_src_details = GST_ELEMENT_DETAILS ("AppSrc",
    "FIXME",
    "FIXME",
    "autogenerated by makefilter");

enum
{
  PROP_0
};

static GstStaticPadTemplate gst_app_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

static void gst_app_src_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_app_src_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);
static void gst_app_src_dispose (GObject * object);
static GstFlowReturn gst_app_src_create (GstPushSrc * psrc, GstBuffer ** buf);
static gboolean gst_app_src_start (GstBaseSrc * psrc);
static gboolean gst_app_src_stop (GstBaseSrc * psrc);
static gboolean gst_app_src_unlock (GstBaseSrc * psrc);

GST_BOILERPLATE (GstAppSrc, gst_app_src, GstPushSrc, GST_TYPE_PUSH_SRC);

static void
gst_app_src_base_init (gpointer g_class)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (g_class);

  //GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);

  GST_DEBUG_CATEGORY_INIT (app_src_debug, "appsrc", 0, "appsrc element");

  gst_element_class_set_details (element_class, &app_src_details);

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_app_src_template));

}

static void
gst_app_src_class_init (GstAppSrcClass * klass)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;
  GstPushSrcClass *pushsrc_class = (GstPushSrcClass *) klass;
  GstBaseSrcClass *basesrc_class = (GstBaseSrcClass *) klass;

  gobject_class->set_property = gst_app_src_set_property;
  gobject_class->get_property = gst_app_src_get_property;
  gobject_class->dispose = gst_app_src_dispose;

  pushsrc_class->create = gst_app_src_create;
  basesrc_class->start = gst_app_src_start;
  basesrc_class->stop = gst_app_src_stop;
  basesrc_class->unlock = gst_app_src_unlock;
}

static void
gst_app_src_dispose (GObject * obj)
{
  GstAppSrc *appsrc = GST_APP_SRC (obj);

  if (appsrc->caps) {
    gst_caps_unref (appsrc->caps);
    appsrc->caps = NULL;
  }
  if (appsrc->mutex) {
    g_mutex_free (appsrc->mutex);
    appsrc->mutex = NULL;
  }
  if (appsrc->cond) {
    g_cond_free (appsrc->cond);
    appsrc->cond = NULL;
  }
  if (appsrc->queue) {
    g_queue_free (appsrc->queue);
    appsrc->queue = NULL;
  }

  G_OBJECT_CLASS (parent_class)->dispose (obj);
}

static void
gst_app_src_init (GstAppSrc * appsrc, GstAppSrcClass * klass)
{
  UNUSEDPAR(klass);
  appsrc->mutex = g_mutex_new ();
  appsrc->cond = g_cond_new ();
  appsrc->queue = g_queue_new ();
}

static void
gst_app_src_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  UNUSEDPAR(value);
  GstAppSrc *appsrc = GST_APP_SRC (object);

  GST_OBJECT_LOCK (appsrc);
  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
  GST_OBJECT_UNLOCK (appsrc);
}

static void
gst_app_src_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec * pspec)
{
  UNUSEDPAR(value);
  GstAppSrc *appsrc = GST_APP_SRC (object);

  GST_OBJECT_LOCK (appsrc);
  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
  GST_OBJECT_UNLOCK (appsrc);
}

static gboolean
gst_app_src_start (GstBaseSrc * psrc)
{
  GstAppSrc *appsrc = GST_APP_SRC (psrc);

  appsrc->unlock = FALSE;

#if 0
  /* FIXME: I don't know if this makes sense */
  appsrc->end_of_stream = FALSE;
  appsrc->flush = FALSE;
#endif

  return TRUE;
}

static gboolean
gst_app_src_stop (GstBaseSrc * psrc)
{
  UNUSEDPAR(psrc);
  //GstAppSrc *appsrc = GST_APP_SRC(psrc);

  return TRUE;
}

static gboolean
gst_app_src_unlock (GstBaseSrc * psrc)
{
  GstAppSrc *appsrc = GST_APP_SRC (psrc);

  appsrc->unlock = TRUE;
  g_cond_signal (appsrc->cond);

  return TRUE;
}

static GstFlowReturn
gst_app_src_create (GstPushSrc * psrc, GstBuffer ** buf)
{
  GstAppSrc *appsrc = GST_APP_SRC (psrc);
  int ret = GST_FLOW_ERROR;

  g_mutex_lock (appsrc->mutex);

  while (1) {
    if (appsrc->unlock) {
      ret = GST_FLOW_WRONG_STATE;
      break;
    }
    if (!g_queue_is_empty (appsrc->queue)) {
      *buf = g_queue_pop_head (appsrc->queue);

      gst_buffer_set_caps (*buf, appsrc->caps);

      ret = GST_FLOW_OK;
      break;
    }
    if (appsrc->end_of_stream) {
      appsrc->end_of_stream = FALSE;
      ret = GST_FLOW_UNEXPECTED;
      break;
    }
    if (appsrc->flush) {
      appsrc->flush = FALSE;
      /* FIXME: I don't really know how to do this */
      break;
    }
    g_cond_wait (appsrc->cond, appsrc->mutex);
  }

  g_mutex_unlock (appsrc->mutex);

  return ret;
}


/* external API */

/**
 * gst_app_src_push_buffer:
 * @appsrc:
 * @buffer:
 *
 * Adds a buffer to the queue of buffers that the appsrc element will
 * push to its source pad.  This function takes ownership of the buffer.
 */
void
gst_app_src_push_buffer (GstAppSrc * appsrc, GstBuffer * buffer)
{
  g_return_if_fail (appsrc);
  g_return_if_fail (GST_IS_APP_SRC (appsrc));

  g_mutex_lock (appsrc->mutex);

  g_queue_push_tail (appsrc->queue, buffer);

  g_cond_signal (appsrc->cond);
  g_mutex_unlock (appsrc->mutex);
}

/**
 * gst_app_src_set_caps:
 * @appsrc:
 * @caps:
 *
 * Set the capabilities on the appsrc element.  This function takes
 * ownership of the caps structure.
 */
void
gst_app_src_set_caps (GstAppSrc * appsrc, GstCaps * caps)
{
  g_return_if_fail (appsrc);
  g_return_if_fail (GST_IS_APP_SRC (appsrc));

  gst_caps_replace (&appsrc->caps, caps);
}

/**
 * gst_app_src_flush:
 * @appsrc:
 *
 * Flushes all queued buffers from the appsrc element.
 */
void
gst_app_src_flush (GstAppSrc * appsrc)
{
  GstBuffer *buffer;

  g_return_if_fail (appsrc);
  g_return_if_fail (GST_IS_APP_SRC (appsrc));

  g_mutex_lock (appsrc->mutex);

  while ((buffer = g_queue_pop_head (appsrc->queue))) {
    gst_buffer_unref (buffer);
  }
  appsrc->flush = TRUE;

  g_cond_signal (appsrc->cond);
  g_mutex_unlock (appsrc->mutex);
}

/**
 * gst_app_src_end_of_stream:
 * @appsrc:
 *
 * Indicates to the appsrc element that the last buffer queued in the
 * element is the last buffer of the stream.
 */
void
gst_app_src_end_of_stream (GstAppSrc * appsrc)
{
  g_return_if_fail (appsrc);
  g_return_if_fail (GST_IS_APP_SRC (appsrc));

  g_mutex_lock (appsrc->mutex);

  appsrc->end_of_stream = TRUE;

  g_cond_signal (appsrc->cond);
  g_mutex_unlock (appsrc->mutex);
}
