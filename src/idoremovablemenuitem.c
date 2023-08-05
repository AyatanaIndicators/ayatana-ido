/*
 * Copyright 2013 Canonical Ltd.
 * Copyright 2023 Robert Tari
 *
 * Authors:
 *   Charles Kerr <charles.kerr@canonical.com>
 *   Robert Tari <robert@tari.in>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>

#include "idoactionhelper.h"
#include "idoremovablemenuitem.h"

enum
{
    PROP_0,
    PROP_ICON,
    PROP_TEXT,
    PROP_LAST
};

static GParamSpec *lProperties[PROP_LAST];

typedef struct
{
    GIcon *pIcon;
    char *sText;
    GtkWidget *pImage;
    GtkWidget *pLabel;
    GtkWidget *pButton;
    gboolean bClosePressed;
    IdoActionHelper *pHelper;

} IdoRemovableMenuItemPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IdoRemovableMenuItem, ido_removable_menu_item, GTK_TYPE_MENU_ITEM);

static void onGetProperty(GObject *pObject, guint nProperty, GValue *pValue, GParamSpec *pParamSpec)
{
    IdoRemovableMenuItem *self = IDO_REMOVABLE_MENU_ITEM(pObject);
    IdoRemovableMenuItemPrivate *pPrivate = ido_removable_menu_item_get_instance_private(self);

    switch (nProperty)
    {
        case PROP_ICON:
        {
            g_value_set_object(pValue, pPrivate->pIcon);

            break;
        }
        case PROP_TEXT:
        {
            g_value_set_string(pValue, pPrivate->sText);

            break;
        }
        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID(pObject, nProperty, pParamSpec);

            break;
        }
    }
}

static void onSetProperty(GObject *pObject, guint nProperty, const GValue *pValue, GParamSpec *pParamSpec)
{
    IdoRemovableMenuItem *self = IDO_REMOVABLE_MENU_ITEM(pObject);

    switch (nProperty)
    {
        case PROP_ICON:
        {
            idoRemovableMenuItemSetIcon(self, g_value_get_object(pValue));

            break;
        }
        case PROP_TEXT:
        {
            idoRemovableMenuItemSetText(self, g_value_get_string(pValue));

            break;
        }
        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID(pObject, nProperty, pParamSpec);

            break;
        }
    }
}

static void onDispose(GObject *pObject)
{
    IdoRemovableMenuItem *self = IDO_REMOVABLE_MENU_ITEM(pObject);
    IdoRemovableMenuItemPrivate *pPrivate = ido_removable_menu_item_get_instance_private(self);

    g_clear_object (&pPrivate->pIcon);
    G_OBJECT_CLASS(ido_removable_menu_item_parent_class)->dispose(pObject);
}

static void onFinalize(GObject * pObject)
{
    IdoRemovableMenuItem *self = IDO_REMOVABLE_MENU_ITEM(pObject);
    IdoRemovableMenuItemPrivate *pPrivate = ido_removable_menu_item_get_instance_private(self);

    g_free(pPrivate->sText);
    G_OBJECT_CLASS (ido_removable_menu_item_parent_class)->finalize(pObject);
}

static void idoRemovableMenuItemStyleUpdateImage(IdoRemovableMenuItem *self)
{
    IdoRemovableMenuItemPrivate *pPrivate = ido_removable_menu_item_get_instance_private(self);

    gtk_image_clear(GTK_IMAGE(pPrivate->pImage));

    if (pPrivate->pIcon == NULL)
    {
        gtk_widget_set_visible(pPrivate->pImage, FALSE);
    }
    else
    {
        gtk_image_set_from_gicon (GTK_IMAGE (pPrivate->pImage), pPrivate->pIcon, GTK_ICON_SIZE_MENU);
        gtk_widget_set_visible (pPrivate->pImage, TRUE);
    }
}

static void onStyleUpdated(GtkWidget *pWidget)
{
    GTK_WIDGET_CLASS(ido_removable_menu_item_parent_class)->style_updated(pWidget);
    idoRemovableMenuItemStyleUpdateImage(IDO_REMOVABLE_MENU_ITEM(pWidget));
    gtk_widget_queue_draw(pWidget);
}

static gboolean idoRemovableMenuItemIsWidget(GtkWidget *pWidget, GdkEventButton *pEventButton)
{
    if (gtk_widget_get_window(pWidget) == NULL)
    {
        return FALSE;
    }

    GtkAllocation cAllocation;
    gtk_widget_get_allocation(pWidget, &cAllocation);

    GdkWindow *window = gtk_widget_get_window(pWidget);

    int nXWin, nYWin;

    gdk_window_get_origin(window, &nXWin, &nYWin);

    int nXMin = cAllocation.x;
    int nXMax = cAllocation.x + cAllocation.width;
    int nYMin = cAllocation.y;
    int nYMax = cAllocation.y + cAllocation.height;
    int nX = pEventButton->x_root - nXWin;
    int nY = pEventButton->y_root - nYWin;

    return nX >= nXMin && nX <= nXMax && nY >= nYMin && nY <= nYMax;
}

static gboolean onButtonPress(GtkWidget *pWidget, GdkEventButton *pEventButton)
{
    g_return_val_if_fail(IDO_IS_REMOVABLE_MENU_ITEM(pWidget), FALSE);

    IdoRemovableMenuItem *self = IDO_REMOVABLE_MENU_ITEM(pWidget);
    IdoRemovableMenuItemPrivate *pPrivate = ido_removable_menu_item_get_instance_private(self);

    if (pEventButton->button == GDK_BUTTON_PRIMARY)
    {
        if (idoRemovableMenuItemIsWidget(pPrivate->pLabel, pEventButton))
        {
            gtk_widget_event(pPrivate->pLabel, (GdkEvent *)pEventButton);
        }
        else if (idoRemovableMenuItemIsWidget(pPrivate->pButton, pEventButton))
        {
            gtk_widget_set_state_flags(pPrivate->pButton, GTK_STATE_FLAG_PRELIGHT | GTK_STATE_FLAG_FOCUSED | GTK_STATE_FLAG_ACTIVE, TRUE);
            pPrivate->bClosePressed = TRUE;
        }
    }

    return TRUE;
}

static gboolean onButtonRelease(GtkWidget *pWidget, GdkEventButton *pEventButton)
{
    g_return_val_if_fail(IDO_IS_REMOVABLE_MENU_ITEM(pWidget), FALSE);

    IdoRemovableMenuItem *self = IDO_REMOVABLE_MENU_ITEM(pWidget);
    IdoRemovableMenuItemPrivate *pPrivate = ido_removable_menu_item_get_instance_private(self);

    if (pEventButton->button == GDK_BUTTON_PRIMARY)
    {
        if (idoRemovableMenuItemIsWidget(pPrivate->pButton, pEventButton))
        {
            if (pPrivate->bClosePressed)
            {
                GtkWidget *pParent = gtk_widget_get_parent(GTK_WIDGET(self));
                GList *lMenuItems = gtk_container_get_children(GTK_CONTAINER(pParent));
                guint nRemovables = 0;

                for(GList *pElem = lMenuItems; pElem; pElem = pElem->next)
                {
                    if (IDO_IS_REMOVABLE_MENU_ITEM(pElem->data))
                    {
                        nRemovables++;
                    }
                }

                g_list_free(lMenuItems);

                if (pPrivate->pHelper)
                {
                    ido_action_helper_activate(pPrivate->pHelper);
                }
                else
                {
                    gtk_container_remove(GTK_CONTAINER(pParent), GTK_WIDGET(self));
                }

                if (GTK_IS_MENU_SHELL(pParent) && nRemovables == 1)
                {
                    gtk_menu_shell_deactivate(GTK_MENU_SHELL(pParent));
                }
            }
        }
        else if (idoRemovableMenuItemIsWidget(pPrivate->pLabel, pEventButton))
        {
            gtk_widget_event(pPrivate->pLabel, (GdkEvent *)pEventButton);
        }
    }

    gtk_widget_set_state_flags(pPrivate->pButton, GTK_STATE_FLAG_NORMAL, TRUE);
    pPrivate->bClosePressed = FALSE;

    return TRUE;
}

static gboolean onMotionNotify(GtkWidget *pMenuItem, GdkEventMotion *pEventMotion)
{
    g_return_val_if_fail(IDO_IS_REMOVABLE_MENU_ITEM(pMenuItem), FALSE);

    IdoRemovableMenuItem *self = IDO_REMOVABLE_MENU_ITEM(pMenuItem);
    IdoRemovableMenuItemPrivate *pPrivate = ido_removable_menu_item_get_instance_private(self);

    GtkAllocation cAllocationSelf;

    gtk_widget_get_allocation(GTK_WIDGET(self), &cAllocationSelf);

    GtkWidget *pWidget;

    if (idoRemovableMenuItemIsWidget(pPrivate->pButton, (GdkEventButton*)pEventMotion))
    {
        pWidget = pPrivate->pButton;

        if (!pPrivate->bClosePressed)
        {
            gtk_widget_set_state_flags(pPrivate->pButton, GTK_STATE_FLAG_FOCUSED, TRUE);
        }
        else
        {
            gtk_widget_set_state_flags(pPrivate->pButton, GTK_STATE_FLAG_PRELIGHT | GTK_STATE_FLAG_FOCUSED | GTK_STATE_FLAG_ACTIVE, TRUE);
        }
    }
    else
    {
        pWidget = pPrivate->pLabel;
        gtk_widget_set_state_flags(pPrivate->pButton, GTK_STATE_FLAG_NORMAL, TRUE);
    }

    GtkAllocation cAllocationLabel;

    gtk_widget_get_allocation(pWidget, &cAllocationLabel);

    GdkEventMotion *pEventMotionNew = (GdkEventMotion *)gdk_event_copy((GdkEvent *)pEventMotion);
    pEventMotionNew->x = pEventMotion->x - (cAllocationLabel.x - cAllocationSelf.x);
    pEventMotionNew->y = pEventMotion->y - (cAllocationLabel.y - cAllocationSelf.y);

    gtk_widget_event(pWidget, (GdkEvent *)pEventMotionNew);
    gdk_event_free((GdkEvent *)pEventMotionNew);

    return FALSE;
}

static gboolean onLeaveNotify(GtkWidget *pWidget, GdkEventCrossing *pEventCrossing)
{
    g_return_val_if_fail(IDO_IS_REMOVABLE_MENU_ITEM(pWidget), FALSE);

    IdoRemovableMenuItem *self = IDO_REMOVABLE_MENU_ITEM(pWidget);
    IdoRemovableMenuItemPrivate *pPrivate = ido_removable_menu_item_get_instance_private(self);

    gtk_widget_event(pPrivate->pLabel, (GdkEvent *)pEventCrossing);

    return FALSE;
}

static void ido_removable_menu_item_class_init(IdoRemovableMenuItemClass *klass)
{
    GObjectClass *pObject = G_OBJECT_CLASS(klass);
    GtkWidgetClass *pWidget = GTK_WIDGET_CLASS(klass);
    GtkMenuItemClass *pMenuItem = GTK_MENU_ITEM_CLASS(klass);
    pObject->get_property = onGetProperty;
    pObject->set_property = onSetProperty;
    pObject->dispose = onDispose;
    pObject->finalize = onFinalize;
    pWidget->style_updated = onStyleUpdated;
    pWidget->leave_notify_event = onLeaveNotify;
    pWidget->motion_notify_event = onMotionNotify;
    pWidget->button_press_event = onButtonPress;
    pWidget->button_release_event = onButtonRelease;
    pMenuItem->select = NULL;

    GParamFlags nParamFlags = G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS;
    lProperties[PROP_ICON] = g_param_spec_object("icon", "Icon", "The menuitem's GIcon", G_TYPE_OBJECT, nParamFlags);
    lProperties[PROP_TEXT] = g_param_spec_string ("text", "Text", "The menuitem's text", "", nParamFlags);

    g_object_class_install_properties(pObject, PROP_LAST, lProperties);
}

static gboolean onActivateLink(GtkLabel *pLabel, gchar *sUri, gpointer pUserData)
{
    g_return_val_if_fail(IDO_IS_REMOVABLE_MENU_ITEM(pUserData), FALSE);

    IdoRemovableMenuItem *self = IDO_REMOVABLE_MENU_ITEM(pUserData);
    GError *pError = NULL;

    if (!gtk_show_uri_on_window(NULL, sUri, gtk_get_current_event_time(), &pError))
    {
        g_warning("Unable to show '%s': %s", sUri, pError->message);
        g_error_free(pError);
    }

    GtkWidget *pParent = gtk_widget_get_parent(GTK_WIDGET(self));

    if (GTK_IS_MENU_SHELL(pParent))
    {
        gtk_menu_shell_deactivate(GTK_MENU_SHELL(pParent));
    }

    return TRUE;
}

static void ido_removable_menu_item_init(IdoRemovableMenuItem *self)
{
    IdoRemovableMenuItemPrivate *pPrivate = ido_removable_menu_item_get_instance_private(self);
    pPrivate->bClosePressed = FALSE;
    pPrivate->pHelper = NULL;
    pPrivate->pImage = gtk_image_new();

    gtk_widget_set_halign(pPrivate->pImage, GTK_ALIGN_START);
    gtk_widget_set_valign(pPrivate->pImage, GTK_ALIGN_START);

    pPrivate->pLabel = gtk_label_new("");

    gtk_widget_set_halign(pPrivate->pLabel, GTK_ALIGN_START);
    gtk_widget_set_valign(pPrivate->pLabel, GTK_ALIGN_CENTER);

    pPrivate->pButton = gtk_button_new();

    gtk_button_set_label(GTK_BUTTON(pPrivate->pButton), "X");
    gtk_widget_set_halign(pPrivate->pButton, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(pPrivate->pButton, GTK_ALIGN_CENTER);
    gtk_widget_show(pPrivate->pButton);

    GtkWidget *pBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(pBox), pPrivate->pImage, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(pBox), pPrivate->pLabel, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(pBox), pPrivate->pButton, FALSE, FALSE, 0);
    g_object_set(pPrivate->pImage, "halign", GTK_ALIGN_START, "hexpand", FALSE, "valign", GTK_ALIGN_CENTER, "margin-right", 6, NULL);
    g_object_set(pPrivate->pLabel, "halign", GTK_ALIGN_START, "hexpand", TRUE, "margin-right", 6, "valign", GTK_ALIGN_CENTER, NULL);
    gtk_widget_show(pBox);
    gtk_container_add(GTK_CONTAINER(self), pBox);
    g_signal_connect(pPrivate->pLabel, "activate-link", G_CALLBACK(onActivateLink), self);
}

GtkWidget *ido_removable_menu_item_new (void)
{
    return GTK_WIDGET(g_object_new(IDO_TYPE_REMOVABLE_MENU_ITEM, NULL));
}

void idoRemovableMenuItemSetIcon(IdoRemovableMenuItem *self, GIcon *pIcon)
{
    IdoRemovableMenuItemPrivate *pPrivate = ido_removable_menu_item_get_instance_private(self);

    if (pPrivate->pIcon != pIcon)
    {
        if (pPrivate->pIcon)
        {
            g_object_unref(pPrivate->pIcon);
        }

        pPrivate->pIcon = pIcon ? g_object_ref(pIcon) : NULL;
        idoRemovableMenuItemStyleUpdateImage(self);
    }
}

void idoRemovableMenuItemSetIconFromFile(IdoRemovableMenuItem *self, const char *sFilename)
{
    GFile *pFile = sFilename ? g_file_new_for_path(sFilename) : NULL;
    GIcon *pIcon = pFile ? g_file_icon_new(pFile) : NULL;

    idoRemovableMenuItemSetIcon(self, pIcon);

    g_clear_object(&pIcon);
    g_clear_object(&pFile);
}

void idoRemovableMenuItemSetText(IdoRemovableMenuItem *self, const char *sText)
{
    IdoRemovableMenuItemPrivate *pPrivate = ido_removable_menu_item_get_instance_private(self);

    if (g_strcmp0(pPrivate->sText, sText))
    {
        g_free(pPrivate->sText);
        pPrivate->sText = g_strdup(sText);

        g_object_set (G_OBJECT(pPrivate->pLabel), "label", pPrivate->sText, "visible", (gboolean)(pPrivate->sText && *pPrivate->sText), NULL);
    }
}

GtkMenuItem *ido_removable_menu_item_new_from_model(GMenuItem *pMenuItem, GActionGroup *pActionGroup)
{
    GtkWidget *pItem = ido_removable_menu_item_new();
    IdoRemovableMenuItemPrivate *pPrivate = ido_removable_menu_item_get_instance_private(IDO_REMOVABLE_MENU_ITEM(pItem));
    gboolean bUseMarkup = FALSE;
    g_menu_item_get_attribute(pMenuItem, "x-ayatana-use-markup", "b", &bUseMarkup);
    idoRemovableMenuItemUseMarkup(IDO_REMOVABLE_MENU_ITEM(pItem), bUseMarkup);

    gchar *sLabel;

    if (g_menu_item_get_attribute(pMenuItem, "label", "s", &sLabel))
    {
        idoRemovableMenuItemSetText(IDO_REMOVABLE_MENU_ITEM(pItem), sLabel);
        g_free(sLabel);
    }

    GVariant *sIcon = g_menu_item_get_attribute_value(pMenuItem, "icon", NULL);

    if (sIcon)
    {
        GIcon *pIcon = g_icon_deserialize(sIcon);
        idoRemovableMenuItemSetIcon(IDO_REMOVABLE_MENU_ITEM(pItem), pIcon);
        g_object_unref(pIcon);
        g_variant_unref(sIcon);
    }

    gchar *sAction;

    if (g_menu_item_get_attribute(pMenuItem, "action", "s", &sAction))
    {
        GVariant *sTarget = g_menu_item_get_attribute_value(pMenuItem, "target", NULL);
        pPrivate->pHelper = ido_action_helper_new(pItem, pActionGroup, sAction, sTarget);
        g_signal_connect_swapped(pItem, "destroy", G_CALLBACK (g_object_unref), pPrivate->pHelper);

        if (sTarget)
        {
            g_variant_unref(sTarget);
        }

        g_free(sAction);
    }

    return GTK_MENU_ITEM(pItem);
}

void idoRemovableMenuItemUseMarkup(IdoRemovableMenuItem *self, gboolean bUse)
{
    IdoRemovableMenuItemPrivate *pPrivate = ido_removable_menu_item_get_instance_private(IDO_REMOVABLE_MENU_ITEM(self));
    g_object_set(pPrivate->pLabel, "use-markup", bUse, NULL);
}
