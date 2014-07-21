/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-ionvif-event-handler.c
 * Copyright (C) 2014 Watson Xu <xuhuashan@gmail.com>
 *
 * ionvif is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * ionvif is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <json-glib/json-glib.h>
#include "ipcam-ionvif-event-handler.h"
#include "ionvif.h"


G_DEFINE_TYPE (IpcamIOnvifEventHandler, ipcam_ionvif_event_handler, IPCAM_EVENT_HANDLER_TYPE);


static void
ipcam_ionvif_event_handler_run_impl(IpcamEventHandler *event_handler, IpcamMessage *message);

static void
ipcam_ionvif_event_handler_init (IpcamIOnvifEventHandler *ipcam_ionvif_event_handler)
{
}

static void
ipcam_ionvif_event_handler_class_init (IpcamIOnvifEventHandlerClass *klass)
{
    IpcamEventHandlerClass *handler_class = IPCAM_EVENT_HANDLER_CLASS(klass);

    handler_class->run = ipcam_ionvif_event_handler_run_impl;
}



static void
ipcam_ionvif_event_handler_run_impl(IpcamEventHandler *event_handler, IpcamMessage *message)
{
    IpcamIOnvif *ionvif;
    const gchar *event;
    JsonNode *body;

    g_object_get(G_OBJECT(message), "event", &event, NULL);
    g_object_get(G_OBJECT(event_handler), "service", &ionvif, NULL);
    g_object_get(G_OBJECT(message), "body", &body, NULL);

    if (g_strcmp0(event, "set_network") == 0)
        ipcam_ionvif_update_network_setting(ionvif, body);
    else if(g_strcmp0(event, "set_datetime") == 0)
        ipcam_ionvif_update_datetime_setting(ionvif, body);
    else if(g_strcmp0(event, "set_base_info") == 0)
        ipcam_ionvif_update_base_info_setting(ionvif, body);
}
