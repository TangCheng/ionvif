/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ionvif.c
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

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ionvif.h"
#include "onvif-server.h"

struct _IpcamIOnvifPrivate
{
	GThread *onvif_thread;
	gboolean terminating;
};




G_DEFINE_TYPE (IpcamIOnvif, ipcam_ionvif, IPCAM_BASE_APP_TYPE);

static void
ipcam_ionvif_init (IpcamIOnvif *ipcam_ionvif)
{
	ipcam_ionvif->priv = G_TYPE_INSTANCE_GET_PRIVATE (ipcam_ionvif, IPCAM_TYPE_IONVIF, IpcamIOnvifPrivate);

	ipcam_ionvif->priv->onvif_thread = NULL;
	ipcam_ionvif->priv->terminating = FALSE;
}

static void
ipcam_ionvif_finalize (GObject *object)
{
	IpcamIOnvif *ionvif = IPCAM_IONVIF(object);
	IpcamIOnvifPrivate *priv = ionvif->priv;

	priv->terminating = TRUE;
	g_thread_join(priv->onvif_thread);

	G_OBJECT_CLASS (ipcam_ionvif_parent_class)->finalize (object);
}

static void ipcam_ionvif_before_start(IpcamBaseService *base_service);
static void ipcam_ionvif_in_loop(IpcamBaseService *base_service);

static void
ipcam_ionvif_class_init (IpcamIOnvifClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IpcamIOnvifPrivate));

	object_class->finalize = ipcam_ionvif_finalize;

	IpcamBaseServiceClass *base_service_class = IPCAM_BASE_SERVICE_CLASS(klass);
	base_service_class->before = ipcam_ionvif_before_start;
	base_service_class->in_loop = ipcam_ionvif_in_loop;
}

static void ipcam_ionvif_before_start(IpcamBaseService *base_service)
{
	IpcamIOnvif *ionvif = IPCAM_IONVIF(base_service);
	IpcamIOnvifPrivate *priv = ionvif->priv;

	priv->onvif_thread = g_thread_new("onvif-server",
	                                  onvif_server_thread_func,
	                                  ionvif);
}

static void ipcam_ionvif_in_loop(IpcamBaseService *base_service)
{
}

gboolean ipcam_ionvif_is_terminating(IpcamIOnvif *ionvif)
{
	return ionvif->priv->terminating;
}

static gchar *get_network_ipaddr(const char *ifname)
{
    struct ifreq ifr;
    int sockfd;
	gchar *ipaddr = NULL;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        return NULL;

	strcpy(ifr.ifr_name, ifname);
	if (ioctl(sockfd, SIOCGIFADDR, &ifr) == 0)
        ipaddr = strdup(inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));

    close(sockfd);

	return ipaddr;
}

gchar *ipcam_ionvif_get_server_addr(IpcamIOnvif *ionvif)
{
	gchar *ipaddr = NULL;
	const gchar *onvif_netif = ipcam_base_app_get_config(IPCAM_BASE_APP(ionvif), "onvif:netif");

	if (onvif_netif)
		ipaddr = get_network_ipaddr(onvif_netif);

	if (ipaddr == NULL)
		ipaddr = g_strdup("127.0.0.1");

	return ipaddr;
}

uint ipcam_ionvif_get_server_port(IpcamIOnvif *ionvif)
{
	int port = 8080;
	const gchar *onvif_port = ipcam_base_app_get_config(IPCAM_BASE_APP(ionvif), "onvif:server-port");

	if (onvif_port)
		port = strtoul(onvif_port, NULL, 0);

	return port;
}

uint ipcam_ionvif_get_rtsp_port(IpcamIOnvif *ionvif)
{
	int port = 554;
	const gchar *onvif_port = ipcam_base_app_get_config(IPCAM_BASE_APP(ionvif), "onvif:rtsp-port");

	if (onvif_port)
		port = strtoul(onvif_port, NULL, 0);

	return port;
}
