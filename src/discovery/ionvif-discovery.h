/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ionvif-discovery.h
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

#ifndef _IONVIF_DISCOVERY_H_
#define _IONVIF_DISCOVERY_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define IPCAM_TYPE_IONVIF_DISCOVERY             (ionvif_discovery_get_type ())
#define IPCAM_IONVIF_DISCOVERY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), IPCAM_TYPE_IONVIF_DISCOVERY, IOnvifDiscovery))
#define IPCAM_IONVIF_DISCOVERY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), IPCAM_TYPE_IONVIF_DISCOVERY, IOnvifDiscoveryClass))
#define IPCAM_IS_IONVIF_DISCOVERY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IPCAM_TYPE_IONVIF_DISCOVERY))
#define IPCAM_IS_IONVIF_DISCOVERY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), IPCAM_TYPE_IONVIF_DISCOVERY))
#define IPCAM_IONVIF_DISCOVERY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), IPCAM_TYPE_IONVIF_DISCOVERY, IOnvifDiscoveryClass))

typedef struct _IOnvifDiscoveryClass IOnvifDiscoveryClass;
typedef struct _IOnvifDiscovery IOnvifDiscovery;
typedef struct _IOnvifDiscoveryPrivate IOnvifDiscoveryPrivate;


struct _IOnvifDiscoveryClass
{
	GObjectClass parent_class;
};

struct _IOnvifDiscovery
{
	GObject parent_instance;

	IOnvifDiscoveryPrivate *priv;
};

GType ionvif_discovery_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* _IONVIF_DISCOVERY_H_ */

