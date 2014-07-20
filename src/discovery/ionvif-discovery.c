/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ionvif-discovery.c
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

#include "ionvif-discovery.h"

struct _IOnvifDiscoveryPrivate
{
	GThread *discovery_thread;
	gboolean terminating;
};




G_DEFINE_TYPE (IOnvifDiscovery, ionvif_discovery, IPCAM_BASE_APP_TYPE);

static void
ionvif_discovery_init (IOnvifDiscovery *ionvif_discovery)
{
	ionvif_discovery->priv = G_TYPE_INSTANCE_GET_PRIVATE (ionvif_discovery, IPCAM_TYPE_IONVIF_DISCOVERY, IOnvifDiscoveryPrivate);

	ionvif_discovery->priv->discovery_thread = NULL;
	ionvif_discovery->priv->terminating = FALSE;
}

static void
ionvif_discovery_finalize (GObject *object)
{
	/* TODO: Add deinitalization code here */

	G_OBJECT_CLASS (ionvif_discovery_parent_class)->finalize (object);
}

static void
ionvif_discovery_class_init (IOnvifDiscoveryClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IOnvifDiscoveryPrivate));

	object_class->finalize = ionvif_discovery_finalize;
}


