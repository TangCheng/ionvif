/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-ionvif-discovery.c
 * Copyright (C) 2014 Watson Xu <watson@watsons-pc>
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

#include "discovery-server.h"
#include "ipcam-ionvif-discovery.h"

struct _IpcamIOnvifDiscoveryPrivate
{
	GThread *discovery_thread;
	gboolean terminating;
};




G_DEFINE_TYPE (IpcamIOnvifDiscovery, ipcam_ionvif_discovery, IPCAM_BASE_APP_TYPE);

static void
ipcam_ionvif_discovery_init (IpcamIOnvifDiscovery *ipcam_ionvif_discovery)
{
	ipcam_ionvif_discovery->priv = G_TYPE_INSTANCE_GET_PRIVATE (ipcam_ionvif_discovery, IPCAM_TYPE_IONVIF_DISCOVERY, IpcamIOnvifDiscoveryPrivate);

	ipcam_ionvif_discovery->priv->discovery_thread = NULL;
	ipcam_ionvif_discovery->priv->terminating = FALSE;
}

static void
ipcam_ionvif_discovery_finalize (GObject *object)
{
	IpcamIOnvifDiscovery *ionvif_discovery = IPCAM_IONVIF_DISCOVERY(object);
	IpcamIOnvifDiscoveryPrivate *priv = ionvif_discovery->priv;

	priv->terminating = TRUE;
	g_thread_join(priv->discovery_thread);

	G_OBJECT_CLASS (ipcam_ionvif_discovery_parent_class)->finalize (object);
}

static void ipcam_ionvif_discovery_before_start(IpcamBaseService *base_service);
static void ipcam_ionvif_discovery_in_loop(IpcamBaseService *base_service);

static void
ipcam_ionvif_discovery_class_init (IpcamIOnvifDiscoveryClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IpcamIOnvifDiscoveryPrivate));

	object_class->finalize = ipcam_ionvif_discovery_finalize;

	IpcamBaseServiceClass *base_service_class = IPCAM_BASE_SERVICE_CLASS(klass);
	base_service_class->before = ipcam_ionvif_discovery_before_start;
	base_service_class->in_loop = ipcam_ionvif_discovery_in_loop;
}

static void ipcam_ionvif_discovery_before_start(IpcamBaseService *base_service)
{
	IpcamIOnvifDiscovery *ionvif_discovery = IPCAM_IONVIF_DISCOVERY(base_service);
	IpcamIOnvifDiscoveryPrivate *priv = ionvif_discovery->priv;

	priv->discovery_thread = g_thread_new("onvif-discovery-server",
	                                      onvif_discovery_server_thread_func,
	                                      ionvif_discovery);
}

static void ipcam_ionvif_discovery_in_loop(IpcamBaseService *base_service)
{
}

gboolean ipcam_ionvif_discovery_is_terminating(IpcamIOnvifDiscovery *ionvif_discovery)
{
	return ionvif_discovery->priv->terminating;
}
